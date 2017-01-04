#include "ImageSymbol.h"
#include "Image.h"
#include "RenderContext.h"
#include "GUM_DTex.h"
#include "DTexC2Strategy.h"

#include <sprite2/Texture.h>

namespace gum
{
 
ImageSymbol::ImageSymbol()
	: m_img(NULL)
	, m_packed(false)
{
	InitTexcoords();
}

ImageSymbol::ImageSymbol(uint32_t id)
	: s2::ImageSymbol(id)
	, m_img(NULL)
	, m_packed(false)
{
	InitTexcoords();
}

ImageSymbol::~ImageSymbol()
{
	if (m_img) {
		m_img->RemoveReference();
	}
}

sm::vec2 ImageSymbol::GetNoTrimedSize() const
{
	if (m_packed) {
		return sm::vec2(m_size.xmax - m_size.xmin, m_size.ymax - m_size.ymin);
	} else {
		return m_tex->GetOriSize();
	}
}

void ImageSymbol::SetImage(Image* img)
{
	cu::RefCountObjAssign(m_img, img);

	sm::i16_rect q;
	q.xmin = q.ymin = 0;
	sm::ivec2 sz = m_img->GetSize();
	q.xmax = sz.x;
	q.ymax = sz.y;

	InitTex(m_img->GetS2Tex(), q, sm::vec2(0, 0));	
}

void ImageSymbol::SetRegion(const sm::ivec2& min, const sm::ivec2& max)
{
	m_packed = true;

	m_rotate = false;
	if (max.y < min.y) {
		m_rotate = true;
	}

	float hw, hh;
	sm::ivec2 sz = m_img->GetSize();
	float txmin, tymin, txmax, tymax;
	txmin = (float)min.x / sz.x;
	tymin = (float)min.y / sz.y;
	txmax = (float)max.x / sz.x;
	tymax = (float)max.y / sz.y;
	if (m_rotate) 
	{
		hw = (min.y - max.y) * 0.5f;
		hh = (max.x - min.x) * 0.5f;

		m_texcoords[0] = txmin; m_texcoords[1] = tymin;
		m_texcoords[2] = txmin; m_texcoords[3] = tymax;
		m_texcoords[4] = txmax; m_texcoords[5] = tymax;
		m_texcoords[6] = txmax; m_texcoords[7] = tymin;

		m_quad.xmin = min.x;
		m_quad.ymin = max.y;
		m_quad.xmax = max.x;
		m_quad.ymax = min.y;
	}
	else
	{
		hw = (max.x - min.x) * 0.5f;
		hh = (max.y - min.y) * 0.5f;

		m_texcoords[0] = txmin; m_texcoords[1] = tymin;
		m_texcoords[2] = txmax; m_texcoords[3] = tymin;
		m_texcoords[4] = txmax; m_texcoords[5] = tymax;
		m_texcoords[6] = txmin; m_texcoords[7] = tymax;

		m_quad.xmin = min.x;
		m_quad.ymin = min.y;
		m_quad.xmax = max.x;
		m_quad.ymax = max.y;
	}

	m_size.xmin = -hw;
	m_size.ymin = -hh;
	m_size.xmax = hw;
	m_size.ymax = hh;
}

void ImageSymbol::QueryTexcoords(float* texcoords, int& texid) const
{
	UID uid = ResourceUID::BinNode(GetID());
	const float* c2_texcoords = DTex::Instance()->QuerySymbol(uid, &texid);
	if (c2_texcoords) {
		memcpy(texcoords, c2_texcoords, sizeof(float) * 8);
		if (m_rotate) {
			float x, y;
			x = texcoords[6]; y = texcoords[7];
			texcoords[6] = texcoords[4]; texcoords[7] = texcoords[5];
			texcoords[4] = texcoords[2]; texcoords[5] = texcoords[3];
			texcoords[2] = texcoords[0]; texcoords[3] = texcoords[1];
			texcoords[0] = x;           texcoords[1] = y;
		}
	} else {
		texid = m_img->GetTexID();
		memcpy(texcoords, m_texcoords, sizeof(m_texcoords));

		sm::ivec2 sz = m_img->GetSize();
		DTexC2Strategy::Instance()->OnC2QueryFail(GetID(), texid, sz.x, sz.y, m_quad);
	}
}

void ImageSymbol::Proj2Screen(float px, float py, int w, int h, float& sx, float& sy) const
{
	sx = w * 0.5f + px;
	sy = h * 0.5f - py;
}

bool ImageSymbol::IsOrthoCam() const
{
	return true;
}

void ImageSymbol::GetScreenSize(int& w, int& h) const
{
	w = RenderContext::Instance()->GetWidth();
	h = RenderContext::Instance()->GetHeight();
}

float ImageSymbol::GetP3dCamAngle() const
{
	return 0;
}

int ImageSymbol::GetScreenCacheTexid() const
{
	// todo
//	return dtexf_cs1_get_texture_id();
	return 0;
}

void ImageSymbol::InitTexcoords()
{
	m_texcoords[0] = 0; m_texcoords[1] = 0;
	m_texcoords[2] = 0; m_texcoords[3] = 1;
	m_texcoords[4] = 1; m_texcoords[5] = 1;
	m_texcoords[6] = 1; m_texcoords[7] = 0;
}

}