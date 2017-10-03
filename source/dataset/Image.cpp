#include "Image.h"
#include "ImageLoader.h"
#include "RenderContext.h"

#include <sprite2/S2_Texture.h>
#include <sprite2/StatImages.h>
#include <unirender/UR_RenderContext.h>

#include <assert.h>

namespace gum
{

static int ALL_IMG_COUNT = 0;

Image::Image()
	: m_pkg_id(s2::StatImages::UNKNOWN_IMG_ID)
	, m_width(0)
	, m_height(0)
	, m_format(-1)
	, m_id(0)
	, m_s2_tex(std::make_shared<s2::Texture>(0, 0, 0))
{
	++ALL_IMG_COUNT;
}

Image::Image(int pkg_id, const ResPath& res_path, bool async)
	: m_pkg_id(pkg_id)
	, m_res_path(res_path)
	, m_width(0)
	, m_height(0)
	, m_format(-1)
	, m_id(0)
	, m_s2_tex(std::make_shared<s2::Texture>(0, 0, 0))
{
	++ALL_IMG_COUNT;

	if (!async)
	{
		ImageLoader loader(m_res_path);
		bool ret = loader.Load();
		if (ret) {
			LoadFromLoader(loader);
			s2::StatImages::Instance()->Add(pkg_id, m_width, m_height, m_format);
		}
	}
}

Image::~Image()
{
	--ALL_IMG_COUNT;

	if (m_id != 0) {
		s2::StatImages::Instance()->Remove(m_pkg_id, m_width, m_height, m_format);
	}

	RenderContext::Instance()->GetImpl()->ReleaseTexture(m_id);
}

void Image::AsyncLoad(int pkg_id, int format, int width, int height)
{
	if (m_id != 0) {
		return;
	}

	m_pkg_id = pkg_id;

	ImageLoader loader(m_res_path);
	loader.AsyncLoad(format, width, height, shared_from_this());

	s2::StatImages::Instance()->Add(pkg_id, width, height, format);
}

bool Image::IsLoadFinished() const 
{ 
	if (m_s2_tex) {
		return m_s2_tex->IsLoadFinished();
	} else {
		return true;
	}
}

void Image::SetLoadFinished(bool finished) 
{ 
	if (m_s2_tex) {
		m_s2_tex->SetLoadFinished(finished);
	}
}

void Image::LoadFromLoader(const ImageLoader& loader)
{
	m_id     = loader.GetID();
	m_format = loader.GetFormat();
	m_width  = loader.GetWidth();
	m_height = loader.GetHeight();

	m_s2_tex->Init(m_width, m_height, m_id);
	m_s2_tex->InitOri(m_width, m_height);
}

int Image::GetAllImgCount()
{
	return ALL_IMG_COUNT;
}

}