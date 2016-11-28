#include "SymbolLoader.h"
#include "SymbolPool.h"

namespace gum
{

s2::Symbol* SymbolLoader::Create(const std::string& filepath, int type) const
{
	return SymbolPool::Instance()->Fetch(filepath, type);
}

}