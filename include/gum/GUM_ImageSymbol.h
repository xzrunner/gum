#ifndef _GUM_IMAGE_SYMBOL_H_
#define _GUM_IMAGE_SYMBOL_H_

#include "gum_config.h"

#include <sprite2/ImageSymbol.h>

#include <string>

namespace simp { class NodePicture; }

namespace gum
{

class Image;

class ImageSymbol : public s2::ImageSymbol
{
public:
	ImageSymbol();
	ImageSymbol(uint32_t id);
	virtual ~ImageSymbol();

	/**
	 *  @interface
	 *    s2::ImageSymbol
	 */
	virtual sm::vec2 GetNoTrimedSize() const;

	/**
	 *  @interface
	 *    s2::ImageSymbol
	 */
	virtual bool QueryTexcoords(bool use_dtex, float* texcoords, int& texid) const;
	virtual void OnQueryTexcoordsFail() const;

	void SetImage(Image* img);
	const Image* GetImage() const { return m_img; }

	void SetRegion(const sm::ivec2& min, const sm::ivec2& max, 
		const sm::vec2& offset, int lod, float scale);

private:
#ifdef GUM_DEBUG
	virtual bool IsProxyImg() const;
#endif // GUM_DEBUG

	void InitTexcoords();

private:
	Image* m_img;

	float m_texcoords[8];

	bool m_packed;

}; // ImageSymbol

}

#endif // _GUM_IMAGE_SYMBOL_H_
