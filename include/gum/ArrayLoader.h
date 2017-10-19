#ifndef _GUM_ARRAY_LOADER_H_
#define _GUM_ARRAY_LOADER_H_

#include <SM_Vector.h>
#include <cu/cu_stl.h>

#include <stdint.h>

namespace gum
{

class ArrayLoader
{
public:
	static void Load(CU_VEC<sm::vec2>& dst, const uint16_t* src, int src_n, int precision);
	static void Load(CU_VEC<int>& dst, const uint16_t* src, int src_n);

	static void Load(CU_VEC<sm::vec2>& dst, const uint32_t* src, int src_n);

}; // ArrayLoader

}

#endif // _GUM_ARRAY_LOADER_H_