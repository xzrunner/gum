#include "gum/Statistics.h"
#include "gum/StatFPS.h"
#include "gum/GUM_GTxt.h"

#include <glp_loop.h>
#include <ps_3d.h>
#include <sprite2/pre_defined.h>
#include S2_MAT_HEADER
#include <sprite2/StatDrawCall.h>
#include <sprite2/StatPingPong.h>
#include <sprite2/StatTopNodes.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Statistics.h>

#include <string>

#include <time.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace gum
{

SINGLETON_DEFINITION(Statistics);

static const float FPS_SMOOTHING = 0.99f;

static const int FLUSH_COUNT = 100;

Statistics::Statistics()
	: m_enable(false)
	, m_tpf(0)
	, m_tpf_smooth(0)
	, m_no_stat_begin(0)
	, m_no_stat_tot(0)
	, m_opt_enable(false)
	, m_fout_flush(0)
{
	memset(&m_mem, 0, sizeof(m_mem));

#ifdef __ANDROID__
	m_fout.open("/sdcard/lr_stat.bin", std::ofstream::out | std::ofstream::binary);
#else
	m_fout.open("lr_stat.bin", std::ofstream::out | std::ofstream::binary);	
#endif // __ANDROID__
}

void Statistics::Update()
{
	if (!m_enable) {
		return;
	}

	m_tpf = glp_get_dt();
	m_tpf_smooth = (m_tpf_smooth * FPS_SMOOTHING) + m_tpf * (1.0f - FPS_SMOOTHING);

	StatFPS::Instance()->Update();
}

void Statistics::PrintGraph() const
{
	if (!m_enable) {
		return;
	}

	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	mgr->SetShader(sl::SPRITE2);

	static char buf[512];

	const int w = 960;
	const int top = 280;

	S2_MAT mt;
	mt.Translate(0, top);

	sprintf(buf, "OPT: %s, emitter: %d", m_opt_enable ? "open" : "closed", p3d_emitter_count());
	GTxt::Instance()->Draw(mt, buf, w);	

	mt.Translate(0, -30);
	sprintf(buf, "FPS: %.1f, ms: %.1f, ex:%.1f", 1000.0f / m_tpf, m_tpf_smooth, m_no_stat_tot / 1000.0f);
	GTxt::Instance()->Draw(mt, buf, w);	

	mt.Translate(0, -30);
	sprintf(buf, "MEM: tot %.1f, tex %.1f, lua %.1f", m_mem.tot, m_mem.tex, m_mem.lua);
	GTxt::Instance()->Draw(mt, buf, w);	

	static std::string buf_str;
	buf_str.reserve(512);

	mt.Translate(0, -30);
	sl::Statistics::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -30);
	s2::StatDrawCall::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -30);
	s2::StatPingPong::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -200);
	s2::StatTopNodes::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mgr->FlushShader();
}

void Statistics::PrintFile()
{
	sl::Statistics* sl_stat = sl::Statistics::Instance();

	static char buf[512];
	sprintf(buf, "timestamp %" PRId64 ", cost %.1f, vertices %d, dc %d\n", 
		time(NULL), m_tpf_smooth, sl_stat->GetVertices(), sl_stat->GetDrawCall());
	m_fout << buf;

	++m_fout_flush;
	if (m_fout_flush == FLUSH_COUNT) {
		m_fout_flush = 0;
		m_fout.flush();
	}
}

void Statistics::Reset()
{
	if (!m_enable) {
		return;
	}

	m_tpf = 0;

	sl::Statistics::Instance()->Reset();

	s2::StatDrawCall::Instance()->Reset();
	s2::StatPingPong::Instance()->Reset();
	s2::StatTopNodes::Instance()->Reset();
}

void Statistics::NoStatBegin()
{
	m_no_stat_begin = glp_get_time();
}

void Statistics::NoStatEnd()
{
	m_no_stat_tot = (glp_get_time() - m_no_stat_begin);
}

void Statistics::OptEnable(bool enable)
{
	m_opt_enable = enable;
}

void Statistics::SetMem(float tot, float lua)
{
	m_mem.tot = tot;
	m_mem.lua = lua;
}

}