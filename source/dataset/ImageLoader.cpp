#include "ImageLoader.h"
#include "RenderContext.h"
#include "GUM_AsyncTask.h"
#include <gum/Config.h>
#include "Image.h"

#include <gimg_import.h>
#include <gimg_typedef.h>
#include <gimg_pvr.h>
#include <gimg_etc2.h>
#include <bimp/FileLoader.h>
#include <bimp/BIMP_ImportStream.h>
#include <timp/TextureFormat.h>
#include <timp/TextureLoader.h>
#include <unirender/UR_RenderContext.h>

#include <stdlib.h>

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

class FileLoader : public bimp::FileLoader
{
public:
	FileLoader(const std::string& filepath, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
		: bimp::FileLoader(filepath, use_cache)
		, m_parser_cb(parser_cb)
		, m_ud(ud)
	{}

protected:
	virtual void OnLoad(bimp::ImportStream& is) 
	{
		m_parser_cb(is.Stream(), is.Size(), m_ud);
	}

private:
	void (*m_parser_cb)(const void* data, size_t size, void* ud);
	void* m_ud;

}; // FileLoader

static 
void _load_cb(const char* filepath, void (*unpack)(const void* data, size_t size, void* ud), void* ud)
{
	FileLoader loader(filepath, false, unpack, ud);
	loader.Load();
}

static 
void _parser_cb(const void* data, size_t size, void* ud)
{
	timp::TextureLoader loader(static_cast<const char*>(data), size);
	loader.Load();

	Image* img = static_cast<Image*>(ud);
	const void* pixels = loader.GetData();
	int width = loader.GetWidth(),
		height = loader.GetHeight();
	int format = loader.GetFormat();
	switch (format)
	{
	case timp::TEXTURE_RGBA4: case timp::TEXTURE_RGBA8:
		RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), pixels, width, height);
		break;
	case timp::TEXTURE_PVR2:
#if defined( __APPLE__ ) && !defined(__MACOSX)
		RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), pixels, width, height);
#endif
		break;
	case timp::TEXTURE_PVR4:
		{
#if defined( __APPLE__ ) && !defined(__MACOSX)
			RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), pixels, width, height);
#else
			uint8_t* uncompressed = gimg_pvr_decode(static_cast<const uint8_t*>(pixels), width, height);
			gimg_revert_y(uncompressed, width, height, GPF_RGBA);
			RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), uncompressed, width, height);
			free(uncompressed);
#endif
		}
		break;
	case timp::TEXTURE_ETC1:
		break;
	case timp::TEXTURE_ETC2:
		{
#ifdef __ANDROID__
			RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), pixels, width, height);
#elif defined( __APPLE__ ) && !defined(__MACOSX)
			RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), pixels, width, height);
#elif defined _WIN32
			RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), pixels, width, height);
#else
			uint8_t* uncompressed = gimg_etc2_decode(static_cast<const uint8_t*>(pixels), width, height, ETC2PACKAGE_RGBA_NO_MIPMAPS);
			RenderContext::Instance()->GetImpl()->UpdateTexture(img->GetTexID(), uncompressed, width, height);
			free(uncompressed);
#endif // __ANDROID__
		}
		break;
	}
	img->SetLoadFinished(true);
}

static
void _release_cb(void* ud)
{
	Image* img = static_cast<Image*>(ud);
	img->RemoveReference();
}

bool ImageLoader::AsyncLoad(int format, int width, int height, Image* img)
{
	if (m_filepath.find(".ept") == std::string::npos) {
		return false;
	}

	int real_fmt = format;
#if defined( __APPLE__ ) && defined(__MACOSX)
	if (real_fmt == timp::TEXTURE_ETC2) {
		real_fmt = timp::TEXTURE_RGBA8;
	}
#endif

	m_id = RenderContext::Instance()->GetImpl()->CreateTextureID(width, height, real_fmt);
	m_format = format;
	m_width = width;
	m_height = height;

	img->SetLoadFinished(false);
	img->AddReference();
	AsyncTask::Instance()->Load(m_filepath, _load_cb, _parser_cb, _release_cb, img);

	return true;
}

bool ImageLoader::LoadRaw()
{
	int w, h, fmt;
	uint8_t* pixels = gimg_import(m_filepath.c_str(), &w, &h, &fmt);
	if (!pixels) {
		return false;
	}

	if (fmt == GPF_RGBA && Config::Instance()->GetPreMulAlpha()) {
		gimg_pre_mul_alpha(pixels, w, h);
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
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, ur::TEXTURE_PVR2);
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
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, ur::TEXTURE_PVR4);
#else
	uint8_t* uncompressed = gimg_pvr_decode(static_cast<const uint8_t*>(data), m_width, m_height);
	gimg_revert_y(uncompressed, m_width, m_height, GPF_RGBA);
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(uncompressed, m_width, m_height, timp::TEXTURE_RGBA8);
	free(uncompressed);
#endif
	return true;
}

bool ImageLoader::DecodeETC2(const void* data)
{
#ifdef __ANDROID__
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, timp::TEXTURE_ETC2);
#elif defined( __APPLE__ ) && !defined(__MACOSX)
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, timp::TEXTURE_ETC2);
#elif defined _WIN32
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, timp::TEXTURE_ETC2);
#else
	uint8_t* uncompressed = gimg_etc2_decode(static_cast<const uint8_t*>(data), m_width, m_height, ETC2PACKAGE_RGBA_NO_MIPMAPS);
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(uncompressed, m_width, m_height, timp::TEXTURE_RGBA8);
	free(uncompressed);
#endif // __ANDROID__
	return true;
}

}
