#include "gum/Image.h"
#include "gum/RenderContext.h"
#include "gum/Texture.h"

#include <painting2/Texture.h>
#include <sprite2/StatImages.h>
#include <unirender/RenderContext.h>
#include <s2loader/ImageLoader.h>

#include <assert.h>

namespace gum
{

static int ALL_IMG_COUNT = 0;

Image::Image()
	: m_pkg_id(s2::StatImages::UNKNOWN_IMG_ID)
	, m_texture(std::make_shared<Texture>(0, 0, 0, 0))
{
	++ALL_IMG_COUNT;
}

Image::Image(int pkg_id, const bimp::FilePath& res_path, bool async)
	: m_pkg_id(pkg_id)
	, m_res_path(res_path)
	, m_texture(std::make_shared<Texture>(0, 0, 0, 0))
{
	++ALL_IMG_COUNT;

	if (!async)
	{
		s2loader::ImageLoader loader(m_res_path);
		bool ret = loader.Load();
		if (ret) {
			LoadFromLoader(loader);
			s2::StatImages::Instance()->Add(
				pkg_id, m_texture->GetWidth(), m_texture->GetHeight(), m_texture->GetFormat());
		}
	}
}

Image::~Image()
{
	--ALL_IMG_COUNT;

	if (m_texture->GetTexID() != 0) {
		s2::StatImages::Instance()->Remove(
			m_pkg_id, m_texture->GetWidth(), m_texture->GetHeight(), m_texture->GetFormat());
	}
}

void Image::AsyncLoad(int pkg_id, int format, int width, int height)
{
	if (m_texture->GetTexID() != 0) {
		return;
	}

	m_pkg_id = pkg_id;

	s2loader::ImageLoader loader(m_res_path);
	loader.AsyncLoad(format, width, height, shared_from_this());

	s2::StatImages::Instance()->Add(pkg_id, width, height, format);
}

sm::ivec2 Image::GetSize() const 
{
	auto sz = m_texture->GetSize();
	return sm::ivec2(sz.x, sz.y);
}

uint32_t Image::GetTexID() const 
{ 
	return m_texture->GetTexID(); 
}

bool Image::IsLoadFinished() const 
{ 
	if (m_texture) {
		return m_texture->IsLoadFinished();
	} else {
		return true;
	}
}

void Image::SetLoadFinished(bool finished) 
{ 
	if (m_texture) {
		m_texture->SetLoadFinished(finished);
	}
}

void Image::LoadFromLoader(const s2loader::ImageLoader& loader)
{
	auto id     = loader.GetID();
	auto format = loader.GetFormat();
	auto width  = loader.GetWidth();
	auto height = loader.GetHeight();

	m_texture->Init(width, height, id, format);
	m_texture->InitOri(width, height);
}

int Image::GetAllImgCount()
{
	return ALL_IMG_COUNT;
}

}