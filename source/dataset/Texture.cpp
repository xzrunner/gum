#include "Texture.h"
#include "RenderContext.h"
#include "gum/StringHelper.h"
#include "gum/Exception.h"

#include <gum/Config.h>
#include <gimg_import.h>
#include <gimg_typedef.h>
#include <unirender/UR_RenderContext.h>

namespace gum
{

Texture::Texture()
	: m_width(0)
	, m_height(0)
	, m_id(0)
{
}

Texture::~Texture()
{
	RenderContext::Instance()->GetImpl()->ReleaseTexture(m_id);
}

void Texture::Load(const CU_STR& filepath)
{
	int w, h, fmt;
	uint8_t* pixels = gimg_import(filepath.c_str(), &w, &h, &fmt);
	if (!pixels) {
		return;
	}

	if (fmt == GPF_RGBA8 && Config::Instance()->GetPreMulAlpha()) {
		gimg_pre_mul_alpha(pixels, w, h);
	}

	ur::TEXTURE_FORMAT tf = ur::TEXTURE_INVALID;
	switch (fmt)
	{
	case GPF_ALPHA: case GPF_LUMINANCE: case GPF_LUMINANCE_ALPHA:
		tf = ur::TEXTURE_A8;
		break;
	case GPF_RGB:
		tf = ur::TEXTURE_RGB;
		break;
	case GPF_RGBA8:
		tf = ur::TEXTURE_RGBA8;
		break;
	}

	m_width = w;
	m_height = h;
	m_format = tf;

	Load(pixels, m_width, m_height, m_format);
	free(pixels);
}

void Texture::Load(const uint8_t* data, int width, int height, ur::TEXTURE_FORMAT format)
{
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, width, height, format);
}

}
