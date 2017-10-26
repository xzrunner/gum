#include "gum/SymbolLoader.h"
#include "gum/SymbolPool.h"

namespace gum
{

s2::SymPtr SymbolLoader::Create(const CU_STR& filepath, int type) const
{
	return SymbolPool::Instance()->Fetch(filepath, type);
}

s2::SymPtr SymbolLoader::Create(int type) const
{
	return nullptr;
}

}