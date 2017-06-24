#include "GUM_ImageSymbol.h"
#include "Image.h"
#include "RenderContext.h"
#include "GUM_DTex.h"
#include "DTexC2Strategy.h"
#include "ProxyImage.h"

#include <shaderlab/ShaderMgr.h>
#include <sprite2/S2_Texture.h>
#include <sprite2/RenderParams.h>

#include <string.h>

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

bool ImageSymbol::QueryTexcoords(bool use_dtex, float* texcoords, int& texid) const
{
	if (!use_dtex) {
		texid = m_img->GetTexID();
		memcpy(texcoords, m_texcoords, sizeof(m_texcoords));
		return true;
	}

	UID uid = ResourceUID::BinNode(GetID());
	const float* c2_texcoords = DTex::Instance()->QuerySymbol(uid, &texid);
	if (c2_texcoords) 
	{
		memcpy(texcoords, c2_texcoords, sizeof(float) * 8);
		if (m_rotate) 
		{
			float x, y;
			x = texcoords[6]; y = texcoords[7];
			texcoords[6] = texcoords[4]; texcoords[7] = texcoords[5];
			texcoords[4] = texcoords[2]; texcoords[5] = texcoords[3];
			texcoords[2] = texcoords[0]; texcoords[3] = texcoords[1];
			texcoords[0] = x;            texcoords[1] = y;
		}
		return true;
	} 
	else 
	{
		texid = m_img->GetTexID();
		memcpy(texcoords, m_texcoords, sizeof(m_texcoords));
		return false;
	}
}

bool ImageSymbol::OnQueryTexcoordsFail() const
{
	if (!DTex::Instance()->IsC2Enable()) {
		return false;
	}

	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	sl::ShaderType type = mgr->GetShaderType();

	sm::ivec2 sz = m_img->GetSize();
	bool loaded = DTexC2Strategy::Instance()->OnC2QueryFail(GetID(), m_img->GetTexID(), sz.x, sz.y, m_region);

	if (loaded) {
		mgr->SetShader(type);
	}

	return loaded;
}

void ImageSymbol::SetImage(Image* img)
{
	cu::RefCountObjAssign(m_img, img);

	sm::i16_rect q;
	q.xmin = q.ymin = 0;
	sm::ivec2 sz = m_img->GetSize();
	q.xmax = sz.x;
	q.ymax = sz.y;

	InitTex(m_img->GetS2Tex(), q);	
}

void ImageSymbol::SetRegion(const sm::ivec2& min, const sm::ivec2& max, 
							const sm::vec2& offset, int lod, float scale)
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

		m_region.xmin = min.x;
		m_region.ymin = max.y;
		m_region.xmax = max.x;
		m_region.ymax = min.y;
	}
	else
	{
		hw = (max.x - min.x) * 0.5f;
		hh = (max.y - min.y) * 0.5f;

		m_texcoords[0] = txmin; m_texcoords[1] = tymin;
		m_texcoords[2] = txmax; m_texcoords[3] = tymin;
		m_texcoords[4] = txmax; m_texcoords[5] = tymax;
		m_texcoords[6] = txmin; m_texcoords[7] = tymax;

		m_region.xmin = min.x;
		m_region.ymin = min.y;
		m_region.xmax = max.x;
		m_region.ymax = max.y;
	}

	hw /= scale;
	hh /= scale;
	if (lod == 1) {
		hw *= 2;
		hh *= 2;
	} else if (lod == 2) {
		hw *= 4;
		hh *= 4;
	}

	m_size.xmin = -hw + offset.x;
	m_size.ymin = -hh + offset.y;
	m_size.xmax = hw + offset.x;
	m_size.ymax = hh + offset.y;
}

#ifdef GUM_DEBUG
bool ImageSymbol::IsProxyImg() const
{
	return dynamic_cast<ProxyImage*>(m_img) != NULL;
}
#endif // GUM_DEBUG

void ImageSymbol::InitTexcoords()
{
	m_texcoords[0] = 0; m_texcoords[1] = 0;
	m_texcoords[2] = 1; m_texcoords[3] = 0;
	m_texcoords[4] = 1; m_texcoords[5] = 1;
	m_texcoords[6] = 0; m_texcoords[7] = 1;
}

}