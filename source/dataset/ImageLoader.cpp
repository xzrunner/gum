#include "ImageLoader.h"
#include "RenderContext.h"

#include <gimg_import.h>
#include <gimg_typedef.h>
#include <gimg_pvr.h>
#include <gimg_etc2.h>
#include <timp/TextureFormat.h>
#include <timp/TextureLoader.h>
#include <unirender/RenderContext.h>

namespace gum
{

ImageLoader::ImageLoader(const std::string& filepath)
	: m_filepath(filepath)
	, m_id(0)
	, m_format(0)
	, m_width(0)
	, m_height(0)
{
}

bool ImageLoader::Load()
{
	if (m_filepath.find(".ept") != std::string::npos) {
		return LoadBin();
	} else {
		return LoadRaw();
	}
}

bool ImageLoader::LoadRaw()
{
	int w, h, fmt;
	uint8_t* pixels = gimg_import(m_filepath.c_str(), &w, &h, &fmt);
	if (!pixels) {
		return false;
	}

	m_width = w;
	m_height = h;

	timp::TEXTURE_FORMAT tf = timp::TEXTURE_INVALID;
	switch (fmt)
	{
	case GPF_ALPHA: case GPF_LUMINANCE: case GPF_LUMINANCE_ALPHA:
		tf = timp::TEXTURE_A8;
		break;
	case GPF_RGB:
		tf = timp::TEXTURE_RGB;
		break;
	case GPF_RGBA:
		tf = timp::TEXTURE_RGBA8;
		break;
	}

	m_format = tf;
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(pixels, w, h, tf);
	free(pixels);

	return true;
}

bool ImageLoader::LoadBin()
{
	bool ret = true;

	timp::TextureLoader loader(m_filepath);
	loader.Load();
	
	m_format = loader.GetFormat();
	m_width  = loader.GetWidth();
	m_height = loader.GetHeight();
	switch (m_format)
	{
	case timp::TEXTURE_RGBA4: case timp::TEXTURE_RGBA8:
		m_id = RenderContext::Instance()->GetImpl()->CreateTexture(static_cast<const uint8_t*>(loader.GetData()), m_width, m_height, m_format);
		break;
	case timp::TEXTURE_PVR2:
		ret = DecodePVR2(loader.GetData());
		break;
	case timp::TEXTURE_PVR4:
		ret = DecodePVR4(loader.GetData());
		break;
	case timp::TEXTURE_ETC1:
		ret = false;
		break;
	case timp::TEXTURE_ETC2:
		ret = DecodeETC2(loader.GetData());
		break;
	}

	return ret;
}

bool ImageLoader::DecodePVR2(const void* data)
{
#if defined( __APPLE__ ) && !defined(__MACOSX)
//	int internal_format = 0;
//	internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(TEXTURE_PVR2, m_width, m_height, data, 0, 0);
	return true;
#else
	return false;
#endif
}

bool ImageLoader::DecodePVR4(const void* data)
{
#if defined( __APPLE__ ) && !defined(__MACOSX)
//	int internal_format = 0;
//	internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, TEXTURE_PVR4);
#else
	uint8_t* uncompressed = gimg_pvr_decode(static_cast<const uint8_t*>(data), m_width, m_height);
	gimg_revert_y((uint32_t*)uncompressed, m_width, m_height);
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(uncompressed, m_width, m_height, timp::TEXTURE_RGBA8);
	free(uncompressed);
#endif
	return true;
}

bool ImageLoader::DecodeETC2(const void* data)
{
#ifdef __ANDROID__
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, TEXTURE_ETC2);
#else
	uint8_t* uncompressed = gimg_etc2_decode(static_cast<const uint8_t*>(data), m_width, m_height, ETC2PACKAGE_RGBA_NO_MIPMAPS);
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(uncompressed, m_width, m_height, timp::TEXTURE_RGBA8);
	free(uncompressed);
#endif // __ANDROID__
	return true;
}

}