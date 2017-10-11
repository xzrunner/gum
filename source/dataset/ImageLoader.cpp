#include "ImageLoader.h"
#include "RenderContext.h"
#include <gum/Config.h>
#include "Image.h"
#include "LoadImageTask.h"
#include "gum/ThreadPool.h"

#include <gimg_import.h>
#include <gimg_typedef.h>
#include <gimg_pvr.h>
#include <gimg_etc2.h>
#include <timp/TextureFormat.h>
#include <timp/TextureLoader.h>
#include <unirender/UR_RenderContext.h>

#include <stdlib.h>

namespace gum
{

ImageLoader::ImageLoader(const ResPath& res_path)
	: m_res_path(res_path)
	, m_id(0)
	, m_format(0)
	, m_width(0)
	, m_height(0)
{
}

bool ImageLoader::Load()
{
	const std::string& filepath = m_res_path.GetFilepath();
	if (filepath.find(".ept") != std::string::npos ||
		filepath.find(".pkg") != std::string::npos) {
		return LoadBin();
	} else {
		return LoadRaw();
	}
}

bool ImageLoader::AsyncLoad(int format, int width, int height, Image* img)
{
	if (m_res_path.IsSingleFile() && 
		m_res_path.GetFilepath().find(".ept") == std::string::npos) {
		return false;
	}

	int real_fmt = format;
	if (real_fmt == timp::TEXTURE_ETC2) {
		ur::RenderContext* rc = RenderContext::Instance()->GetImpl();
		if (!rc->IsSupportETC2()) {
			real_fmt = timp::TEXTURE_RGBA4;
		}
	}

	m_id = RenderContext::Instance()->GetImpl()->CreateTextureID(width, height, real_fmt);
	m_format = format;
	m_width = width;
	m_height = height;

	img->LoadFromLoader(*this);

	ThreadPool::Instance()->Run(LoadImageTaskMgr::Instance()->Fetch(img));

	return true;
}

bool ImageLoader::LoadRaw()
{
	int w, h, fmt;
	uint8_t* pixels = gimg_import(m_res_path.GetFilepath().c_str(), &w, &h, &fmt);
	if (!pixels) {
		return false;
	}

	if (fmt == GPF_RGBA8 && Config::Instance()->GetPreMulAlpha()) {
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
	case GPF_RGBA8:
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
	if (m_res_path.IsSingleFile()) 
	{
		timp::TextureLoader loader(m_res_path.GetFilepath());
		loader.Load();
		return LoadBin(loader);
	} 
	else 
	{
		fs_file* file = fs_open(m_res_path.GetFilepath().c_str(), "rb");
		timp::TextureLoader loader(file, m_res_path.GetOffset());
		loader.Load();
		fs_close(file);
		return LoadBin(loader);
	}
}

bool ImageLoader::LoadBin(const timp::TextureLoader& loader)
{
	bool ret = true;

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
	uint16_t* uncompressed = gimg_pvr_decode(static_cast<const uint8_t*>(data), m_width, m_height);
	gimg_revert_y((uint8_t*)uncompressed, m_width, m_height, GPF_RGBA4);
	m_id = RenderContext::Instance()->GetImpl()->CreateTexture(uncompressed, m_width, m_height, timp::TEXTURE_RGBA8);
	free(uncompressed);
#endif
	return true;
}

bool ImageLoader::DecodeETC2(const void* data)
{
	ur::RenderContext* rc = RenderContext::Instance()->GetImpl();
	if (rc->IsSupportETC2()) {
		m_id = rc->CreateTexture(data, m_width, m_height, timp::TEXTURE_ETC2);
	} else {
		uint16_t* uncompressed = gimg_etc2_decode(static_cast<const uint8_t*>(data), m_width, m_height, ETC2PACKAGE_RGBA_NO_MIPMAPS);
		m_id = rc->CreateTexture(uncompressed, m_width, m_height, timp::TEXTURE_RGBA4);
		free(uncompressed);
	}
	return true;
}

}
