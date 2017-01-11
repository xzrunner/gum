#include "ArrayLoader.h"

#include <simp/from_int.h>

namespace gum
{

void ArrayLoader::Load(std::vector<sm::vec2>& dst, const uint16_t* src, int src_n, int precision)
{
	dst.reserve(src_n);
	int idx = 0;
	for (int i = 0; i < src_n; ++i) {
		float x = simp::int2float(int16_t(src[idx++]), precision),
			  y = simp::int2float(int16_t(src[idx++]), precision);
		dst.push_back(sm::vec2(x, y));
	}	
}

void ArrayLoader::Load(std::vector<int>& dst, const uint16_t* src, int src_n)
{
	dst.reserve(src_n);
	for (int i = 0; i < src_n; ++i) {
		dst.push_back(src[i]);
	}
}

}