#ifndef _GUM_FILEPATH_HELPER_H_
#define _GUM_FILEPATH_HELPER_H_

#include <cu/cu_stl.h>

namespace gum
{

class FilepathHelper
{
public:
	static CU_STR Relative(const CU_STR& base, const CU_STR& path);
	static CU_STR Absolute(const CU_STR& base, const CU_STR& path);

	static CU_STR Dir(const CU_STR& path);
	static CU_STR Filename(const CU_STR& path);

	static CU_STR Format(const CU_STR& path);

	template<typename T>
	static T Extension(const T& path);

	static bool Exists(const CU_STR& path);

}; // FilepathHelper

}

#include "gum/FilepathHelper.inl"

#endif // _GUM_FILEPATH_HELPER_H_
