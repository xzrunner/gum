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
	for (int i = 0; i < 4; ++i) {
		dtex::DebugDraw::Draw(RT->GetTexID(i), i + 1);
	}
}

}