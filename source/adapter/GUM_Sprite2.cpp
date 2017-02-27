#include "GUM_Sprite2.h"

#include <sprite2/RenderTargetMgr.h>
#include <sprite2/RenderTarget.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/RenderParams.h>
#include <sprite2/DrawNode.h>
#include <dtex2/DebugDraw.h>

#include <stdint.h>

namespace gum
{

SINGLETON_DEFINITION(Sprite2);

static const uint32_t FLAG_USE_DTEX                = 0x00010000;
static const uint32_t FLAG_DTEX_FORCE_CACHED       = 0x00020000;
static const uint32_t FLAG_DTEX_FORCE_CACHED_DIRTY = 0x00040000;

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

static void _init_spr_flags(s2::Sprite* spr)
{
	spr->SetUserFlag(FLAG_USE_DTEX, true);
	spr->SetUserFlag(FLAG_DTEX_FORCE_CACHED, false);
	spr->SetUserFlag(FLAG_DTEX_FORCE_CACHED_DIRTY, false);
}

static void prepare_render_params(const s2::RenderParams& parent, 
								  const s2::Sprite* spr, 
								  s2::RenderParams& child)
{
	if (parent.IsDisableDTexC2()) {
		child.SetDisableDTexC2(true);
	} else {
		child.SetDisableDTexC2(!spr->GetUserFlag(FLAG_USE_DTEX));
	}
}

void Sprite2::Init()
{
	s2::Sprite::InitHook(_init_spr_flags);
	s2::DrawNode::InitDTexCB(prepare_render_params);
}

bool Sprite2::IsUseDTex(const s2::Sprite* spr)
{
	return spr->GetUserFlag(FLAG_USE_DTEX);
}

void Sprite2::SetUseDTex(const s2::Sprite* spr, bool use)
{
	spr->SetUserFlag(FLAG_USE_DTEX, use);
}

bool Sprite2::IsDTexForceCached(const s2::Sprite* spr)
{
	return spr->GetUserFlag(FLAG_DTEX_FORCE_CACHED);
}

void Sprite2::SetDTexForceCached(const s2::Sprite* spr, bool force)
{
	spr->SetUserFlag(FLAG_DTEX_FORCE_CACHED, force);
}

bool Sprite2::IsDTexForceCachedDirty(const s2::Sprite* spr)
{
	return spr->GetUserFlag(FLAG_DTEX_FORCE_CACHED_DIRTY);
}

void Sprite2::SetDTexForceCachedDirty(const s2::Sprite* spr, bool dirty)
{
	spr->SetUserFlag(FLAG_DTEX_FORCE_CACHED_DIRTY, dirty);
}

}