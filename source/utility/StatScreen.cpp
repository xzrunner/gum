#include "gum/StatScreen.h"
#include "gum/RenderTarget.h"
#include "gum/RenderContext.h"
#include "gum/StringHelper.h"

#include <gimg_typedef.h>
#include <gimg_export.h>
#include <unirender/RenderContext.h>
#include <dtex2/DebugDraw.h>

#include <time.h>
#include <string.h>

namespace gum
{

CU_SINGLETON_DEFINITION(StatScreen);

StatScreen::StatScreen()
	: m_enable(false)
	, m_rt(nullptr)
	, m_x(0)
	, m_y(0)
{
}

void StatScreen::Enable(bool enable)
{
	if (m_enable == enable) {
		return;
	}

	m_enable = enable;

	if (enable) {
		m_rt = new RenderTarget(RT_EDGE, RT_EDGE);
#ifdef __ANDROID__
		CU_STR filepath = "/sdcard/lr_stat_screen.bin";
#else
		CU_STR filepath = "lr_stat_screen.bin";
#endif // __ANDROID__
		m_fout.open(filepath.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app);
	} else {
		delete m_rt;

		m_fout.close();
	}
}

void StatScreen::Print(const RenderTarget* src)
{
	if (!m_enable) {
		return;
	}

	time_t t = time(nullptr);
	m_timestamps[m_y * GRID_COUNT + m_x] = t;

	sm::rect r_src;
	r_src.xmin = r_src.ymin = 0;
	r_src.xmax = r_src.ymax = 1;

	sm::rect r_dst;
	static const float GRID_TEXCOORD_EDGE = 1.0f / GRID_COUNT;
	r_dst.xmin = static_cast<float>(m_x) / GRID_COUNT;
	r_dst.xmax = r_dst.xmin + GRID_TEXCOORD_EDGE;
	r_dst.ymin = static_cast<float>(m_y) / GRID_COUNT;
	r_dst.ymax = r_dst.ymin + GRID_TEXCOORD_EDGE;

	m_rt->Bind();
	src->Draw(r_src, r_dst, RT_EDGE, RT_EDGE);
	m_rt->Unbind();
	
	// to next grid
	++m_x;
	if (m_x == GRID_COUNT) {
		m_x = 0;
		++m_y;
	}
	if (m_y == GRID_COUNT) {
		m_y = 0;
	}
}

void StatScreen::Clear()
{
	m_x = m_y = 0;
	memset(m_timestamps, 0, sizeof(m_timestamps));
}

void StatScreen::Flush()
{
	if (!m_enable) {
		return;
	}

	time_t first_time = m_timestamps[0];

	for (int y = 0; y < GRID_COUNT; ++y) {
		for (int x = 0; x < GRID_COUNT; ++x) {
			int idx = y * GRID_COUNT + x;
			time_t time = m_timestamps[idx];
			if (time != 0) {
				m_fout << "timestamp " << time << ", x " << x << ", y " << y << ", file " << first_time << ".png" << '\n';
			}
		}
	}
	m_fout.flush();

	int next = m_y * GRID_COUNT + m_x;

	m_rt->Bind();

	uint8_t* pixels = new uint8_t[RT_EDGE * RT_EDGE * 3];
	memset(pixels, 0, RT_EDGE * RT_EDGE * 3);
	RenderContext::Instance()->GetImpl()->ReadPixels(pixels, 3, 0, 0, RT_EDGE, RT_EDGE);
	CU_STR filepath = StringHelper::ToString(first_time) + ".png";
	gimg_export(filepath.c_str(), pixels, RT_EDGE, RT_EDGE, GPF_RGB, true);
	delete[] pixels;

	m_rt->Unbind();
}

void StatScreen::DebugDraw() const
{
	if (m_enable) {
		int texid = m_rt->GetTexID();
		dtex::DebugDraw::Draw(texid, 1);
	}
}

}