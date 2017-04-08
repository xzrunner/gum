#include "ProxyImage.h"
#include "StringHelper.h"

#include <sprite2/S2_Texture.h>

namespace gum
{

ProxyImage::ProxyImage(int id, int w, int h, int format)
{
	Init(id, w, h, format);
}

ProxyImage::~ProxyImage()
{
// 	m_id = 0; // avoid release in Image
// 	          // managed by other
}

void ProxyImage::Init(int id, int w, int h, int format)
{
	m_width  = w;
	m_height = h;
	m_format = format;
	m_id     = id;

	m_filepath = GetFilepath(id);

	m_s2_tex->Init(w, h, id);	
}

std::string ProxyImage::GetFilepath(int id)
{
	return "_proxy" + StringHelper::ToString(id);
}

}