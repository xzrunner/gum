#include "gum/Sprite2.h"
#include "gum/DTex.h"
#include "gum/RenderTargetMgr.h"
#include "gum/RenderTarget.h"
#include "gum/Audio.h"
#include "gum/ThreadPool.h"

#include <painting2/RenderTargetMgr.h>
#include <painting2/RenderTarget.h>
#include <painting2/Blackboard.h>
#include <painting2/RenderContext.h>
#include <sprite2/Sprite.h>
#include <sprite2/Symbol.h>
#include <sprite2/RenderParams.h>
#include <sprite2/DrawNode.h>
#include <sprite2/AudioContext.h>
#include <sprite2/Callback.h>
#include <sprite2/Blackboard.h>
#include <dtex2/DebugDraw.h>
#include <shaderlab/Statistics.h>

#include <stdint.h>

namespace gum
{

CU_SINGLETON_DEFINITION(Sprite2);

Sprite2::Sprite2()
	: m_curr_dc(std::numeric_limits<int>::max())
	, m_dc_count(0)
{
}

void Sprite2::DebugDraw() const
{
	auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();
	for (int i = 0; i < 4; ++i) {
		int texid = rt_mgr.GetTexID(i);
		if (texid < 0) {
			continue;
		}
		dtex::DebugDraw::Draw(texid, i + 1);
	}
}

static void prepare_render_params(const s2::RenderParams& parent, 
								  const s2::Sprite* spr, 
								  s2::RenderParams& child)
{
	if (parent.IsDisableDTexC2() || spr->IsDTexDisable()) {
		child.SetDisableDTexC2(true);
	}
}

static void
dtex_sym_insert(s2loader::UID uid, const sm::rect& bounding, int tex_id, int tex_w, int tex_h)
{
	DTex* dtex = DTex::Instance();
	dtex->LoadSymStart();

	sm::i16_rect r_dst;
	r_dst.xmin = static_cast<uint16_t>(bounding.xmin + tex_w * 0.5f);
	r_dst.ymin = static_cast<uint16_t>(bounding.ymin + tex_h * 0.5f);
	r_dst.xmax = static_cast<uint16_t>(bounding.xmax + tex_w * 0.5f);
	r_dst.ymax = static_cast<uint16_t>(bounding.ymax + tex_h * 0.5f);

	dtex->LoadSymbol(uid, tex_id, tex_w, tex_h, r_dst, 1, 0, 0);

	dtex->LoadSymFinish();
}

static const float* 
dtex_sym_query(s2loader::UID uid, int& tex_id, int& block_id)
{
	return DTex::Instance()->QuerySymbol(uid, tex_id, block_id);
}

static uint64_t 
get_sym_uid(const s2::Symbol& sym)
{
	return s2loader::ResourceUID::BinNode(sym.GetID());
}

static uint64_t 
get_spr_uid(const s2::Sprite* spr)
{
	return s2loader::ResourceUID::Sprite(spr->GetID());
}

static uint64_t 
get_actor_uid(const s2::Actor* actor)
{
	return s2loader::ResourceUID::Actor(actor);
}

static pt2::RenderTarget* 
fetch_screen()
{
	auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();
	return rt_mgr.Fetch();
}

static void 
return_screen(pt2::RenderTarget* rt)
{
	auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();
	rt_mgr.Return(static_cast<RenderTarget*>(rt));
}

static bool
is_audio_enable()
{
	return Audio::Instance()->IsEnable();
}

static void
submit_task(mt::Task* task)
{
	ThreadPool::Instance()->Run(task);
}

static int
query_thread_idx(std::thread::id id)
{
	return ThreadPool::Instance()->QueryThreadIdx(id);
}

void Sprite2::Init()
{
	s2::DrawNode::InitDTexCB(prepare_render_params, dtex_sym_insert, dtex_sym_query);
	s2::DrawNode::InitUIDCB(get_sym_uid, get_spr_uid, get_actor_uid);

	auto& rt_mgr = pt2::Blackboard::Instance()->GetRenderContext().GetRTMgr();
	rt_mgr.InitScreenCB(fetch_screen, return_screen);

	{
		s2::AudioContext::Callback cb;
		cb.is_enable = is_audio_enable;
		s2::AudioContext::InitCallback(cb);
	}
	{
		s2::Callback::Funs funs;
		funs.submit_task      = submit_task;
		funs.query_thread_idx = query_thread_idx;
		s2::Callback::RegisterCallback(funs);
	}
}

void Sprite2::Flush()
{
	//int dc = sl::Statistics::Instance()->GetLastDrawCall();
	//if (dc < 4) {
	//	return;
	//}

	//if (dc < m_curr_dc) {
	//	m_curr_dc = dc;
	//	m_dc_count = 0;
	//} else if (dc == m_curr_dc) {
	//	++m_dc_count;
	//	if (m_dc_count == MAX_DC_COUNT) {
	//		s2::Blackboard::Instance()->SetDlistEnable(true);
	//	}
	//}
}

}