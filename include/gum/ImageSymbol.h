#ifndef _GUM_IMAGE_SYMBOL_H_
#define _GUM_IMAGE_SYMBOL_H_

#include "gum/gum_config.h"

#include <sprite2/ImageSymbol.h>

namespace simp { class NodePicture; }

namespace gum
{

class Image;

class ImageSymbol : public s2::ImageSymbol
{
public:
	ImageSymbol();
	ImageSymbol(uint32_t id);

	/**
	 *  @interface
	 *    s2::ImageSymbol
	 */
	virtual sm::vec2 GetNoTrimedSize() const override;

	/**
	 *  @interface
	 *    s2::ImageSymbol
	 */
	virtual bool QueryTexcoords(bool use_dtex, float* texcoords, int& texid) const override;
	virtual void OnQueryTexcoordsFail(int thread_idx) const override;

	void SetImage(const std::shared_ptr<Image>& img);
	const std::shared_ptr<Image>& GetImage() const { return m_img; }

	void SetRegion(const sm::ivec2& min, const sm::ivec2& max, 
		const sm::vec2& offset, int lod, float scale);

	void SetCacheDirty(int block_id);

private:
	void InitTexcoords();

	void ClearCache();

private:
	std::shared_ptr<Image> m_img = nullptr;

	float m_texcoords[8];

	mutable float m_cached_texcoords[8];
	mutable int   m_cached_texid;
	mutable int   m_cached_block_id;

	bool m_packed;

}; // ImageSymbol

}

#endif // _GUM_IMAGE_SYMBOL_H_
