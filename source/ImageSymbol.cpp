#include "gum/ImageSymbol.h"
#include "gum/Image.h"
#include "gum/ProxyImage.h"
#include "gum/RenderContext.h"
#ifdef S2_MULTITHREAD
#include "gum/ThreadPool.h"
#else
#include "gum/DTexC2Strategy.h"
#endif // S2_MULTITHREAD
#include "gum/DTex.h"
#include "gum/Texture.h"

#include <painting2/Texture.h>
#include <sprite2/RenderParams.h>
#ifndef S2_MULTITHREAD
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Blackboard.h>
#endif  // S2_MULTITHREAD
#ifndef S2_DISABLE_DEFERRED
#include <cooking/Facade.h>
#endif // S2_DISABLE_DEFERRED

#include <string.h>

namespace gum
{
 
ImageSymbol::ImageSymbol()
	: m_packed(false)
{
	InitTexcoords();
	ClearCache();
}

ImageSymbol::ImageSymbol(uint32_t id)
	: s2::ImageSymbol(id)
	, m_packed(false)
{
	InitTexcoords();
	ClearCache();
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
	if (!use_dtex || !m_tex->IsLoadFinished()) {
		texid = m_img->GetTexID();
		memcpy(texcoords, m_texcoords, sizeof(m_texcoords));
		return true;
	}

	if (m_cached_texid < 0)
	{
		s2loader::UID uid = s2loader::ResourceUID::BinNode(GetID());
		int block_id;
		const float* c2_texcoords = gum::DTex::Instance()->QuerySymbol(uid, texid, block_id);
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

			memcpy(m_cached_texcoords, texcoords, sizeof(m_cached_texcoords));
			m_cached_texid = texid;
			m_cached_block_id = block_id;

			return true;
		}
		else
		{
			texid = m_img->GetTexID();
			memcpy(texcoords, m_texcoords, sizeof(m_texcoords));
			return false;
		}
	}
	else
	{
		texid = m_cached_texid;
		memcpy(texcoords, m_cached_texcoords, sizeof(m_cached_texcoords));
		return true;
	}
}

void ImageSymbol::OnQueryTexcoordsFail(cooking::DisplayList* dlist) const
{
	if (!gum::DTex::Instance()->IsC2Enable()) {
		return;
	}

#ifdef S2_MULTITHREAD
	cooking::update_dtex_c2(dlist, GetID(), m_img->GetTexID(), sz.x, sz.y, m_region);
#else
	sl::ShaderMgr* mgr = sl::Blackboard::Instance()->GetShaderMgr();
	sl::ShaderType type = mgr->GetShaderType();
	if (gum::DTexC2Strategy::Instance()->OnC2QueryFail(GetID(), m_img->GetTexID(), m_img->GetWidth(), m_img->GetHeight(), m_region)) {
		mgr->SetShader(type);
	}
#endif // S2_MULTITHREAD
}

void ImageSymbol::SetImage(const std::shared_ptr<Image>& img)
{
	m_img = img;

	sm::i16_rect q;
	q.xmin = q.ymin = 0;
	q.xmax = m_img->GetWidth();
	q.ymax = m_img->GetHeight();

	InitTex(m_img->GetTexture(), q);
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
	auto w = m_img->GetWidth();
	auto h = m_img->GetHeight();
	float txmin, tymin, txmax, tymax;
	txmin = (float)min.x / w;
	tymin = (float)min.y / h;
	txmax = (float)max.x / w;
	tymax = (float)max.y / h;
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

void ImageSymbol::SetCacheDirty(int block_id)
{
	if (block_id == -1 || m_cached_block_id == block_id) {
		ClearCache();
	}
}

void ImageSymbol::InitTexcoords()
{
	m_texcoords[0] = 0; m_texcoords[1] = 0;
	m_texcoords[2] = 1; m_texcoords[3] = 0;
	m_texcoords[4] = 1; m_texcoords[5] = 1;
	m_texcoords[6] = 0; m_texcoords[7] = 1;
}

void ImageSymbol::ClearCache()
{
	memset(m_cached_texcoords, 0, sizeof(m_cached_texcoords));
	m_cached_texid = -1;
	m_cached_block_id = -1;
}

}