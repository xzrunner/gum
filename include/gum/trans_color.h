#ifndef	_GUM_TRANS_COLOR_H_
#define _GUM_TRANS_COLOR_H_

#include <sprite2/Color.h>
#include <cu/cu_stl.h>
#include <bsn/ColorParser.h>

namespace gum
{

s2::Color str2color(const CU_STR& str, bsn::PIXEL_TYPE type);
CU_STR color2str(const s2::Color& col, bsn::PIXEL_TYPE type);

s2::Color int2color(uint32_t i, bsn::PIXEL_TYPE type);
uint32_t color2int(const s2::Color& col, bsn::PIXEL_TYPE type);
uint32_t color2int(const uint8_t rgba[4], bsn::PIXEL_TYPE type);

}

#endif // _GUM_TRANS_COLOR_H_
