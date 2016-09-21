#ifndef	_GUM_TRANS_COLOR_H_
#define _GUM_TRANS_COLOR_H_

#include <sprite2/Color.h>

#include <string>

namespace gum
{

// text		RGBA	old

// text		ARGB
// add		ABGR
// multi	BGRA
enum PixelType
{
	RGBA = 0,
	ARGB,
	ABGR,
	BGRA
};

s2::Color str2color(const std::string& str, PixelType type);
std::string color2str(const s2::Color& col, PixelType type);

s2::Color int2color(uint32_t i, PixelType type);
uint32_t color2int(const s2::Color& col, PixelType type);
uint32_t color2int(const uint8_t rgba[4], PixelType type);

}

#endif // _GUM_TRANS_COLOR_H_
