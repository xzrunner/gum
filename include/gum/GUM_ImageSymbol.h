#ifndef _GUM_IMAGE_SYMBOL_H_
#define _GUM_IMAGE_SYMBOL_H_

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
	virtual bool QueryTexcoords(const s2::RenderParams& rp, float* texcoords, int& texid) const;

	void SetImage(Image* img);
	const Image* GetImage() const { return m_img; }

	void SetRegion(const sm::ivec2& min, const sm::ivec2& max, const sm::vec2& offset);

private:
	void InitTexcoords();

private:
	Image* m_img;

	float m_texcoords[8];

	bool m_packed;

}; // ImageSymbol

}

#endif // _GUM_IMAGE_SYMBOL_H_