#include "gum/trans_color.h"

#include <assert.h>
#include <stdlib.h>

namespace gum
{

pt2::Color str2color(const CU_STR& str, sns::PIXEL_TYPE type)
{
	pt2::Color ret;
	ret.FromRGBA(sns::ColorParser::StringToRGBA(str.c_str(), str.size(), type));
	return ret;
}

CU_STR color2str(const pt2::Color& col, sns::PIXEL_TYPE type)
{
	return sns::ColorParser::RGBAToString(col.ToRGBA(), type).c_str();
}

pt2::Color int2color(uint32_t i, sns::PIXEL_TYPE type)
{
	pt2::Color col;
	col.FromRGBA(sns::ColorParser::Trans(i, type, sns::RGBA));
	return col;
}

uint32_t color2int(const pt2::Color& col, sns::PIXEL_TYPE type)
{
	return sns::ColorParser::Trans(col.ToRGBA(), sns::RGBA, type);
}

uint32_t color2int(const uint8_t rgba[4], sns::PIXEL_TYPE type)
{
	return sns::ColorParser::Trans(pt2::Color(rgba[0], rgba[1], rgba[2], rgba[3]).ToRGBA(), sns::RGBA, type);
}

}