#include "SymbolLoader.h"
#include "SymbolPool.h"

namespace gum
{

s2::SymPtr SymbolLoader::Create(const std::string& filepath, int type) const
{
	return SymbolPool::Instance()->Fetch(filepath, type);
}

s2::SymPtr SymbolLoader::Create(int type) const
{
	return NULL;
}

}