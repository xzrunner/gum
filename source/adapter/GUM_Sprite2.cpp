#include "GUM_Sprite2.h"

#include <unirender/RenderTarget.h>
#include <unirender/Texture.h>
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
	ur::RenderTarget* rt = s2::RenderTarget::Instance()->GetRT0();
	dtex::DebugDraw::Draw(rt->GetTexture()->ID(), 3);
}

}