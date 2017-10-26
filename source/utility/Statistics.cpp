#include "gum/Statistics.h"
#include "gum/StatFPS.h"
#include "gum/StatTag.h"
#include "gum/GTxt.h"
#include "gum/SymbolPool.h"
#include "gum/Image.h"
#include "gum/ImagePool.h"

#include <logger.h>
#include <glp_loop.h>
#include <ps_3d.h>
#include <sprite2/pre_defined.h>
#include S2_MAT_HEADER
#include <sprite2/StatDrawCall.h>
#include <sprite2/StatPingPong.h>
#include <sprite2/StatTopNodes.h>
#include <sprite2/StatSymDraw.h>
#include <sprite2/StatSymCount.h>
#include <sprite2/StatSprCount.h>
#include <sprite2/StatImages.h>
#include <sprite2/Sprite.h>
#include <sprite2/Actor.h>
#include <sprite2/Symbol.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Statistics.h>
#include <shaderlab/StatDrawCall.h>

#include <time.h>

namespace gum
{

CU_SINGLETON_DEFINITION(Statistics);

static const float FPS_SMOOTHING = 0.99f;

Statistics::Statistics()
	: m_flags(0)
	, m_tpf(0)
	, m_tpf_smooth(0)
	, m_no_stat_begin(0)
	, m_no_stat_tot(0)
	, m_opt_enable(false)
{
	memset(&m_mem, 0, sizeof(m_mem));

	EnableConsole(true);
}

void Statistics::EnableGraph(bool enable)
{
	if (enable) {
		m_flags |= FLAG_PRINT_GRAPH;
	} else {
		m_flags &= ~FLAG_PRINT_GRAPH;
	}
}

void Statistics::EnableConsole(bool enable)
{
	if (enable) {
		m_flags |= FLAG_PRINT_CONSOLE;
	} else {
		m_flags &= ~FLAG_PRINT_CONSOLE;
	}
}

void Statistics::EnableFile(bool enable)
{
	if (enable == IsFileEnable()) {
		return;
	}

	if (enable) 
	{
		m_flags |= FLAG_PRINT_FILE;

#ifdef __ANDROID__
		m_fout.open("/sdcard/lr_stat.bin", std::ofstream::out | std::ofstream::binary);
#else
		m_fout.open("lr_stat.bin", std::ofstream::out | std::ofstream::binary);	
#endif // __ANDROID__
	} 
	else 
	{
		m_flags &= ~FLAG_PRINT_FILE;

		m_fout.close();
	}
}

bool Statistics::IsGraphEnable() const
{
	return (m_flags & FLAG_PRINT_GRAPH) != 0;
}

bool Statistics::IsConsoleEnable() const
{
	return (m_flags & FLAG_PRINT_CONSOLE) != 0;	
}

bool Statistics::IsFileEnable() const
{
	return (m_flags & FLAG_PRINT_FILE) != 0;
}

void Statistics::Update()
{
	if (m_flags == 0) {
		return;
	}

	m_tpf = glp_get_dt();
	m_tpf_smooth = (m_tpf_smooth * FPS_SMOOTHING) + m_tpf * (1.0f - FPS_SMOOTHING);
}

void Statistics::Print()
{
	StatTag::Instance()->PrintScreen();

	if (m_flags == 0) {
		return;
	}

	if (m_flags & FLAG_PRINT_GRAPH) {
		PrintScreen();
	}
	if (m_flags & FLAG_PRINT_CONSOLE) {
		PrintConsole();
	}
	if (m_flags & FLAG_PRINT_FILE) {
		PrintFile();
	}
}

void Statistics::Reset()
{
	m_tpf = 0;

	sl::Statistics::Instance()->Reset();
	sl::StatDrawCall::Instance()->Reset();

	s2::StatDrawCall::Instance()->Reset();
	s2::StatPingPong::Instance()->Reset();
	s2::StatTopNodes::Instance()->Reset();
	s2::StatSymDraw::Instance()->Reset();
}

void Statistics::Flush()
{
	StatTag::Instance()->Flush();
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

void Statistics::PrintScreen() const
{
	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	mgr->SetShader(sl::SPRITE2);

	static char buf[512];

	const int w = 960;
	const float top = 280;

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

	static CU_STR buf_str;
	buf_str.reserve(512);

	mt.Translate(0, -30);
	sl::Statistics::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -30);
	s2::StatDrawCall::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -40);
	s2::StatPingPong::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -50);
	sprintf(buf, "COUNT: sym %d, spr %d, actor %d, img %d", 
		s2::Symbol::GetAllSymCount(),
		s2::Sprite::GetAllSprCount(), 
		s2::Actor::GetAllActorCount(), 
		Image::GetAllImgCount());
	GTxt::Instance()->Draw(mt, buf, w);	

	mt.Translate(0, -30);
	s2::StatSymCount::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -30);
	s2::StatSprCount::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	mt.Translate(0, -30);
	s2::StatImages::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

	// memory detail
	{
		const CU_MAP<int, float>& pkg2mem = s2::StatImages::Instance()->GetID2Mem();
		CU_MAP<float, int> mem2pkg;
		CU_MAP<int, float>::const_iterator itr = pkg2mem.begin();
		for ( ; itr != pkg2mem.end(); ++itr) {
			mem2pkg.insert(std::make_pair(itr->second, itr->first));
		}
		CU_MAP<float, int>::const_reverse_iterator itr2 = mem2pkg.rbegin();
		static const int ROW_COUNT = 12;
		mt.Translate(0, -10);
		for (int i = 0; itr2 != mem2pkg.rend(); ++itr2, ++i)
		{
			if (i == ROW_COUNT) {
				i = 0;
				mt.Translate(150, 20 * ROW_COUNT - 20);
			} else {
				mt.Translate(0, -20);
			}
			sprintf(buf, "%d <-> %.1f", itr2->second, itr2->first);
			GTxt::Instance()->Draw(mt, buf, w);	
		}
	}

	mt.SetTransformation(450, 100, 0, 1, 1, 0, 0, 0, 0);
	s2::StatTopNodes::Instance()->Print(buf_str);
	GTxt::Instance()->Draw(mt, buf_str, w);	
	buf_str.clear();

// 	mt.Translate(450, 180);
// 	s2::StatSymbol::Instance()->Print(buf_str);
// 	GTxt::Instance()->Draw(mt, buf_str, w);	
// 	buf_str.clear();

// 	mt.Translate(0, -230);
// 	sl::StatDrawCall::Instance()->Print(buf_str);
// 	GTxt::Instance()->Draw(mt, buf_str, w);	
// 	buf_str.clear();

	mgr->FlushShader();
}

void Statistics::PrintConsole() const
{
	static const int PRINT_COUNT = 30;
	static int count = 0;
	++count;
	if (count == PRINT_COUNT) 
	{
		count = 0;
		sl::Statistics* stat = sl::Statistics::Instance();
		int dc = stat->GetDrawCall();
		int count = stat->GetVertices();
		float fps_curr = StatFPS::Instance()->GetFPSCurr();
		float curr_cost = 0;
		if (fps_curr != 0) {
			curr_cost = 1000.0f / fps_curr;
		}
		//LOGI("fps %.1f, cost %.1f, dc %d, vertices %d, cost avg %.1f, debug count %d\n", 
		//	1000.0f / m_tpf_smooth, m_tpf_smooth, dc, count, curr_cost, s2::SprActors::GetCount());
		//s2::SprActors::ResetCount();

		LOGI("fps %.1f, cost %.1f, dc %d, vertices %d, cost avg %.1f\n",
			1000.0f / m_tpf_smooth, m_tpf_smooth, dc, count, curr_cost);
	}
}

void Statistics::PrintFile() const
{
	sl::Statistics* sl_stat = sl::Statistics::Instance();

	static char buf[512];
	sprintf(buf, "timestamp %lu, cost %.1f, vertices %d, dc %d\n", 
		time(nullptr), m_tpf_smooth, sl_stat->GetVertices(), sl_stat->GetDrawCall());
	m_fout << buf;

// 	static CU_STR buf_str;
// 	buf_str.reserve(1024);
// 	s2::StatTopNodes::Instance()->Print(buf_str);
// 	m_fout << buf_str;

	static const int FLUSH_COUNT = 100;
	static int count = 0;
	++count;
	if (count == FLUSH_COUNT) {
		count = 0;
		m_fout.flush();
	}
}

}