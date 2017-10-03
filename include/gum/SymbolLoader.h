#ifndef _GUM_SYMBOL_LOADER_H_
#define _GUM_SYMBOL_LOADER_H_

#include <sprite2/SymType.h>
#include <sprite2/s2_typedef.h>

#include <string>

namespace gum
{

class SymbolLoader
{
public:
	virtual s2::SymPtr Create(const std::string& filepath, int type = s2::SYM_UNKNOWN) const;
	virtual s2::SymPtr Create(int type) const;

}; // SymbolLoader

}

#endif // _GUM_SYMBOL_LOADER_H_