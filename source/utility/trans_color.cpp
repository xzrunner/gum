#include "gum/trans_color.h"

#include <assert.h>
#include <stdlib.h>

namespace gum
{

//#ifdef __APPLE__

static inline 
char* _itoa(unsigned i, char *a, unsigned r)
{
	if (i/r > 0) a = _itoa(i/r,a,r);
	*a = "0123456789ABCDEF"[i%r];
	return a+1;
}

static inline
char* itoa(int i, char *a, int r)
{
	if ((r < 2) || (r > 16)) r = 10;
	if (i < 0)
	{
		*a = '-';
		*_itoa(-i,a+1,r) = 0;
	}
	else *_itoa(i,a,r) = 0;
	return a;
}

//#endif // __APPLE__

s2::Color str2color(const CU_STR& str, bsn::PIXEL_TYPE type)
{
	s2::Color ret;
	ret.FromRGBA(bsn::ColorParser::StringToRGBA(str.c_str(), type));
	return ret;
}

static inline
char hex2char(int v) 
{
	assert(v >= 0 && v <= 15);
	if (v >= 0 && v <= 9)
		return '0' + v;
	else if (v >= 10 && v <= 15)
		return 'a' + (v - 10);
	else {
		assert(0);
		return '0';
	}
}

static inline
CU_STR channel2char(int col) 
{
	assert(col >= 0 && col <= 255);

	int high = col / 16,
		low  = col % 16;

	CU_STR ret;
	ret += hex2char(high);
	ret += hex2char(low);

	return ret;
}

CU_STR color2str(const s2::Color& col, bsn::PIXEL_TYPE type)
{
	CU_STR ret = "0x";
	if (type == bsn::RGBA) {
		ret += channel2char(col.r);
		ret += channel2char(col.g);
		ret += channel2char(col.b);
		ret += channel2char(col.a);
	} else if (type == bsn::ARGB) {
		ret += channel2char(col.a);
		ret += channel2char(col.r);
		ret += channel2char(col.g);
		ret += channel2char(col.b);
	} else if (type == bsn::ABGR) {
		ret += channel2char(col.a);
		ret += channel2char(col.b);
		ret += channel2char(col.g);
		ret += channel2char(col.r);
	} else if (type == bsn::BGRA) {
		ret += channel2char(col.b);
		ret += channel2char(col.g);
		ret += channel2char(col.r);
		ret += channel2char(col.a);
	}
	return ret;
}

s2::Color int2color(uint32_t i, bsn::PIXEL_TYPE type)
{
	s2::Color col;
	col.FromRGBA(bsn::ColorParser::Trans(i, type, bsn::RGBA));
	return col;
}

uint32_t color2int(const s2::Color& col, bsn::PIXEL_TYPE type)
{
	return bsn::ColorParser::Trans(col.ToRGBA(), bsn::RGBA, type);
}

uint32_t color2int(const uint8_t rgba[4], bsn::PIXEL_TYPE type)
{
	return bsn::ColorParser::Trans(s2::Color(rgba[0], rgba[1], rgba[2], rgba[3]).ToRGBA(), bsn::RGBA, type);
}

}
