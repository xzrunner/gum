#include "GUM_Sprite2.h"
#include "GUM_DTex.h"

#include <sprite2/RenderTargetMgr.h>
#include <sprite2/RenderTarget.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/RenderParams.h>
#include <sprite2/DrawNode.h>
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

static void c2_insert_spr(const s2::Sprite* spr, int tex_id, int tex_w, int tex_h)
{
	DTex* dtex = DTex::Instance();
	dtex->LoadSymStart();

	UID uid = ResourceUID::Sprite(spr->GetID());

	sm::rect r_src = spr->GetSymbol()->GetBounding(spr);
	sm::i16_rect r_dst;
	r_dst.xmin = r_src.xmin + tex_w * 0.5f;
	r_dst.ymin = r_src.ymin + tex_h * 0.5f;
	r_dst.xmax = r_src.xmax + tex_w * 0.5f;
	r_dst.ymax = r_src.ymax + tex_h * 0.5f;

	dtex->LoadSymbol(uid, tex_id, tex_w, tex_h, r_dst, 1, 0, 0);

	dtex->LoadSymFinish();
}

static const float* c2_query_spr(const s2::Sprite* spr, int* tex_id)
{
	UID uid = ResourceUID::Sprite(spr->GetID());
	return DTex::Instance()->QuerySymbol(uid, tex_id);
}

void Sprite2::Init()
{
	s2::DrawNode::InitDTexCB(prepare_render_params, c2_insert_spr, c2_query_spr);
}

}