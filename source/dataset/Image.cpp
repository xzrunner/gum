#include "Image.h"
#include "ImageLoader.h"
#include "RenderContext.h"

#include <sprite2/S2_Texture.h>
#include <sprite2/StatImages.h>
#include <unirender/UR_RenderContext.h>

#include <assert.h>

namespace gum
{

Image::Image()
	: m_width(0)
	, m_height(0)
	, m_id(0)
	, m_s2_tex(new s2::Texture(0, 0, 0))
{
}

Image::~Image()
{
	if (m_id != 0) {
		s2::StatImages::Instance()->Remove(m_width, m_height, m_format);
	}

	RenderContext::Instance()->GetImpl()->ReleaseTexture(m_id);
	if (m_s2_tex) {
		m_s2_tex->RemoveReference();
	}
}

bool Image::LoadFromFile(const ResPath& res_path, bool async)
{
	assert(m_res_path != res_path);

	m_res_path = res_path;

	if (async) {
		return true;
	}

	ImageLoader loader(m_res_path);
	bool ret = loader.Load();
	if (!ret) {
		return false;
	}

	LoadFromLoader(loader);

	s2::StatImages::Instance()->Add(m_width, m_height, m_format);

	return true;
}

void Image::AsyncLoad(int format, int width, int height)
{
	if (m_id != 0) {
		return;
	}

	ImageLoader loader(m_res_path);
	loader.AsyncLoad(format, width, height, this);

	s2::StatImages::Instance()->Add(width, height, format);
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

}