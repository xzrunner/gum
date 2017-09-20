#include "GUM_Sprite2.h"
#include "GUM_DTex.h"
#include "RenderTargetMgr.h"
#include "RenderTarget.h"
#include "GUM_Audio.h"
#include "gum/ThreadPool.h"

#include <sprite2/S2_RenderTargetMgr.h>
#include <sprite2/S2_RenderTarget.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/RenderParams.h>
#include <sprite2/DrawNode.h>
#include <sprite2/AudioContext.h>
#include <sprite2/Callback.h>
#include <dtex2/DebugDraw.h>

#include <stdint.h>

namespace gum
{

SINGLETON_DEFINITION(Sprite2);

Sprite2::Sprite2()
{
}

void Sprite2::DebugDraw() const
{
	s2::RenderTargetMgr* RT = s2::RenderTargetMgr::Instance();
	for (int i = 0; i < 4; ++i) {
		int texid = RT->GetTexID(i);
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
dtex_sym_insert(UID uid, const sm::rect& bounding, int tex_id, int tex_w, int tex_h)
{
	DTex* dtex = DTex::Instance();
	dtex->LoadSymStart();

	sm::i16_rect r_dst;
	r_dst.xmin = bounding.xmin + tex_w * 0.5f;
	r_dst.ymin = bounding.ymin + tex_h * 0.5f;
	r_dst.xmax = bounding.xmax + tex_w * 0.5f;
	r_dst.ymax = bounding.ymax + tex_h * 0.5f;

	dtex->LoadSymbol(uid, tex_id, tex_w, tex_h, r_dst, 1, 0, 0);

	dtex->LoadSymFinish();
}

static const float* 
dtex_sym_query(UID uid, int& tex_id, int& block_id)
{
	return DTex::Instance()->QuerySymbol(uid, tex_id, block_id);
}

static uint64_t 
get_sym_uid(const s2::Symbol* sym)
{
	return ResourceUID::BinNode(sym->GetID());
}

static uint64_t 
get_spr_uid(const s2::Sprite* spr)
{
	return ResourceUID::Sprite(spr->GetID());
}

static uint64_t 
get_actor_uid(const s2::Actor* actor)
{
	return ResourceUID::Actor(actor);
}

static s2::RenderTarget* 
fetch_screen()
{
	return RenderTargetMgr::Instance()->Fetch();
}

static void 
return_screen(s2::RenderTarget* rt)
{
	RenderTargetMgr::Instance()->Return(static_cast<gum::RenderTarget*>(rt));
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

void Sprite2::Init()
{
	s2::DrawNode::InitDTexCB(prepare_render_params, dtex_sym_insert, dtex_sym_query);
	s2::DrawNode::InitUIDCB(get_sym_uid, get_spr_uid, get_actor_uid);

	s2::RenderTargetMgr::Instance()->InitScreenCB(fetch_screen, return_screen);

	{
		s2::AudioContext::Callback cb;
		cb.is_enable = is_audio_enable;
		s2::AudioContext::InitCallback(cb);
	}
	{
		s2::Callback::Funs funs;
		funs.submit_task = submit_task;
		s2::Callback::RegisterCallback(funs);
	}
}

}