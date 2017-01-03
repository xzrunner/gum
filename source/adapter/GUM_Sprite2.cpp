#include "GUM_Sprite2.h"

#include <sprite2/RenderTargetMgr.h>
#include <sprite2/RenderTarget.h>
#include <dtex2/DebugDraw.h>

namespace gum
{

SINGLETON_DEFINITION(Sprite2);

Sprite2::Sprite2()
{
}

void Sprite2::DebugDraw() const
{
	s2::RenderTargetMgr* RT = s2::RenderTargetMgr::Instance();
	s2::RenderTarget* rt = RT->Fetch();
	if (rt) {
		dtex::DebugDraw::Draw(rt->GetTexID(), 1);
		RT->Return(rt);
	}
}

}