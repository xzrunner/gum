#include "trans_color.h"

#include <assert.h>
#include <stdlib.h>

namespace gum
{

static inline 
int char2hex(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else {
		assert(0);
		return 0;
	}
}

static inline 
int char2channel(char high, char low)
{
	int col = char2hex(high) * 16 + char2hex(low);
	assert(col >= 0 && col <= 255);
	return col;
}

s2::Color str2color(const std::string& str, s2::PIXEL_TYPE type)
{
	std::string snum = str;

	if (snum.empty()) {
		return s2::Color();
	}
	if (snum == "0xffffffff") {
		return s2::Color(255, 255, 255, 255);
	}

	if (snum[0] != '0' || (snum[1] != 'x' && snum[1] != 'X'))
	{
		int n = atoi(snum.c_str());
		char buffer[33];
		_itoa(n, buffer, 16);
		snum = "0x" + std::string(buffer);
	}

	int len = snum.length();

	s2::Color ret(0, 0, 0, 1);
	if (len == 4)
	{
		if (type == s2::RGBA || s2::BGRA)
			ret.a = char2channel(snum[2], snum[3]);
		else if (type == s2::ARGB)
			ret.b = char2channel(snum[2], snum[3]);
		else if (type == s2::ABGR)
			ret.r = char2channel(snum[2], snum[3]);
	}
	else if (len == 10)
	{
		if (type == s2::RGBA) {
			ret.r = char2channel(snum[2], snum[3]);
			ret.g = char2channel(snum[4], snum[5]);
			ret.b = char2channel(snum[6], snum[7]);
			ret.a = char2channel(snum[8], snum[9]);
		} else if (type == s2::ARGB) {
			ret.a = char2channel(snum[2], snum[3]);
			ret.r = char2channel(snum[4], snum[5]);
			ret.g = char2channel(snum[6], snum[7]);
			ret.b = char2channel(snum[8], snum[9]);
		} else if (type == s2::ABGR) {
			ret.a = char2channel(snum[2], snum[3]);
			ret.b = char2channel(snum[4], snum[5]);
			ret.g = char2channel(snum[6], snum[7]);
			ret.r = char2channel(snum[8], snum[9]);
		}  else if (type == s2::BGRA) {
			ret.b = char2channel(snum[2], snum[3]);
			ret.g = char2channel(snum[4], snum[5]);
			ret.r = char2channel(snum[6], snum[7]);
			ret.a = char2channel(snum[8], snum[9]);
		}
	}

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
std::string channel2char(int col) 
{
	assert(col >= 0 && col <= 255);

	int high = col / 16,
		low  = col % 16;

	std::string ret;
	ret += hex2char(high);
	ret += hex2char(low);

	return ret;
}

std::string color2str(const s2::Color& col, s2::PIXEL_TYPE type)
{
	std::string ret = "0x";
	if (type == s2::RGBA) {
		ret += channel2char(col.r);
		ret += channel2char(col.g);
		ret += channel2char(col.b);
		ret += channel2char(col.a);
	} else if (type == s2::ARGB) {
		ret += channel2char(col.a);
		ret += channel2char(col.r);
		ret += channel2char(col.g);
		ret += channel2char(col.b);
	} else if (type == s2::ABGR) {
		ret += channel2char(col.a);
		ret += channel2char(col.b);
		ret += channel2char(col.g);
		ret += channel2char(col.r);
	} else if (type == s2::BGRA) {
		ret += channel2char(col.b);
		ret += channel2char(col.g);
		ret += channel2char(col.r);
		ret += channel2char(col.a);
	}
	return ret;
}

s2::Color int2color(uint32_t i, s2::PIXEL_TYPE type)
{
	s2::Color col;
	col.FromRGBA(s2::trans_color(i, type, s2::RGBA));
	return col;
}

uint32_t color2int(const s2::Color& col, s2::PIXEL_TYPE type)
{
	return s2::trans_color(col.ToRGBA(), s2::RGBA, type);
}

uint32_t color2int(const uint8_t rgba[4], s2::PIXEL_TYPE type)
{
	return s2::trans_color(s2::Color(rgba[0], rgba[1], rgba[2], rgba[3]).ToRGBA(), s2::RGBA, type);
}

}