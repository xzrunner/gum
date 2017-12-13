#ifndef _GUM_TEXT_TABLE_LOADER_H_
#define _GUM_TEXT_TABLE_LOADER_H_

#include <cu/cu_stl.h>

namespace gum
{

class TextTableLoader
{
public:
	static void LoadFromCSV(const CU_STR& filepath);

}; // TextTableLoader

}

#endif // _GUM_TEXT_TABLE_LOADER_H_