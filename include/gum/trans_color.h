#ifndef	_GUM_TRANS_COLOR_H_
#define _GUM_TRANS_COLOR_H_

#include <sprite2/Color.h>
#include <sprite2/s2_trans_color.h>

#include <string>

namespace gum
{

s2::Color str2color(const std::string& str, s2::PIXEL_TYPE type);
std::string color2str(const s2::Color& col, s2::PIXEL_TYPE type);

s2::Color int2color(uint32_t i, s2::PIXEL_TYPE type);
uint32_t color2int(const s2::Color& col, s2::PIXEL_TYPE type);
uint32_t color2int(const uint8_t rgba[4], s2::PIXEL_TYPE type);

}

#endif // _GUM_TRANS_COLOR_H_
