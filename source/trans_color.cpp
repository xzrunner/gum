#include "gum/trans_color.h"

#include <assert.h>
#include <stdlib.h>

namespace gum
{

pt2::Color str2color(const CU_STR& str, s2s::PIXEL_TYPE type)
{
	pt2::Color ret;
	ret.FromRGBA(s2s::ColorParser::StringToRGBA(str.c_str(), str.size(), type));
	return ret;
}

CU_STR color2str(const pt2::Color& col, s2s::PIXEL_TYPE type)
{
	return s2s::ColorParser::RGBAToString(col.ToRGBA(), type).c_str();
}

pt2::Color int2color(uint32_t i, s2s::PIXEL_TYPE type)
{
	pt2::Color col;
	col.FromRGBA(s2s::ColorParser::Trans(i, type, s2s::RGBA));
	return col;
}

uint32_t color2int(const pt2::Color& col, s2s::PIXEL_TYPE type)
{
	return s2s::ColorParser::Trans(col.ToRGBA(), s2s::RGBA, type);
}

uint32_t color2int(const uint8_t rgba[4], s2s::PIXEL_TYPE type)
{
	return s2s::ColorParser::Trans(pt2::Color(rgba[0], rgba[1], rgba[2], rgba[3]).ToRGBA(), s2s::RGBA, type);
}

}
