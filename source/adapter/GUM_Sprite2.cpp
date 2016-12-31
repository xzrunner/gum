#include "GUM_Sprite2.h"

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
	s2::RenderTarget* RT = s2::RenderTarget::Instance();
	dtex::DebugDraw::Draw(RT->GetTexID(0), 1);
	dtex::DebugDraw::Draw(RT->GetTexID(1), 2);
	dtex::DebugDraw::Draw(RT->GetTexID(2), 3);
	dtex::DebugDraw::Draw(RT->GetTexID(3), 4);
}

}