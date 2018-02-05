#include "gum/Texture.h"
#include "gum/RenderContext.h"

#include <unirender/RenderContext.h>

namespace gum
{

Texture::Texture(uint16_t w, uint16_t h, uint32_t id, int format)
	: pt2::Texture(w, h, id, format)
{
}

Texture::~Texture()
{
	gum::RenderContext::Instance()->GetImpl()->ReleaseTexture(GetTexID());
}

}
