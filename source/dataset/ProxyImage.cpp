#include "gum/ProxyImage.h"
#include "gum/StringHelper.h"

#include <painting2/Texture.h>

namespace gum
{

ProxyImage::ProxyImage(int id, int w, int h, int format)
{
	Init(id, w, h, format);
}

ProxyImage::~ProxyImage()
{
	m_id = 0; // avoid release in Image
	          // managed by other
}

void ProxyImage::Init(int id, int w, int h, int format)
{
	m_width  = w;
	m_height = h;
	m_format = format;
	m_id     = id;

	m_res_path.SetFilepath(GetFilepath(id));

	m_s2_tex->Init(w, h, id);	
}

CU_STR ProxyImage::GetFilepath(int id)
{
	return CU_STR("_proxy") + StringHelper::ToString(id);
}

}