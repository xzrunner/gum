#include "Image.h"
#include "ImageLoader.h"
#include "RenderContext.h"

#include <sprite2/S2_Texture.h>
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
	RenderContext::Instance()->GetImpl()->ReleaseTexture(m_id);
	if (m_s2_tex) {
		m_s2_tex->RemoveReference();
	}
}

bool Image::LoadFromFile(const std::string& filepath)
{
	assert(m_filepath != filepath);
	m_filepath = filepath;

	ImageLoader loader(m_filepath);
	bool ret = loader.Load();
	if (!ret) {
		return false;
	}

	m_id     = loader.GetID();
	m_format = loader.GetFormat();
	m_width  = loader.GetWidth();
	m_height = loader.GetHeight();

	m_s2_tex->Init(m_width, m_height, m_id);
	m_s2_tex->InitOri(m_width, m_height);

	return true;
}

}