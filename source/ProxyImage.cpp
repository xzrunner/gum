#include "gum/ProxyImage.h"
#include "gum/StringHelper.h"
#include "gum/Texture.h"

namespace gum
{

ProxyImage::ProxyImage(int id, int w, int h, int format)
{
	Init(id, w, h, format);
}

ProxyImage::~ProxyImage()
{
	// avoid release in Image
	// managed by other
	m_texture->Init(0, 0, 0, 0);
}

void ProxyImage::Init(int id, int w, int h, int format)
{
	m_res_path.SetFilepath(GetFilepath(id));

	m_texture->Init(w, h, id, format);	
}

CU_STR ProxyImage::GetFilepath(int id)
{
	return CU_STR("_proxy") + gum::StringHelper::ToString(id);
}

}