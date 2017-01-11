#ifndef _GUM_SYMBOL_LOADER_H_
#define _GUM_SYMBOL_LOADER_H_

#include <CU_RefCountObj.h>

#include <sprite2/SymType.h>

#include <string>

namespace s2 { class Symbol; }

namespace gum
{

class SymbolLoader : public cu::RefCountObj
{
public:
	virtual s2::Symbol* Create(const std::string& filepath, int type = s2::SYM_UNKNOWN) const;
	virtual s2::Symbol* Create(int type) const;

}; // SymbolLoader

}

#endif // _GUM_SYMBOL_LOADER_H_