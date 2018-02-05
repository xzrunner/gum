#ifndef	_GUM_TRANS_COLOR_H_
#define _GUM_TRANS_COLOR_H_

#include <painting2/Color.h>
#include <cu/cu_stl.h>
#include <s2s/ColorParser.h>

namespace gum
{

pt2::Color str2color(const CU_STR& str, s2s::PIXEL_TYPE type);
CU_STR color2str(const pt2::Color& col, s2s::PIXEL_TYPE type);

pt2::Color int2color(uint32_t i, s2s::PIXEL_TYPE type);
uint32_t color2int(const pt2::Color& col, s2s::PIXEL_TYPE type);
uint32_t color2int(const uint8_t rgba[4], s2s::PIXEL_TYPE type);

}

#endif // _GUM_TRANS_COLOR_H_
