#ifndef _GUM_TEXTURE_POLYGON_H_
#define _GUM_TEXTURE_POLYGON_H_

#include <sprite2/TexturePolygon.h>
#include <cu/cu_stl.h>

namespace gum
{

class Image;

class TexturePolygon : public s2::TexturePolygon
{
public:
	TexturePolygon(const CU_STR& filepath);

}; // TexturePolygon

}

#endif // _GUM_TEXTURE_POLYGON_H_
