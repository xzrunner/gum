#include "SymbolLoader.h"
#include "SymbolFactory.h"

namespace gum
{

s2::Symbol* SymbolLoader::Create(const std::string& filepath, int type) const
{
	return SymbolFactory::Instance()->Create(filepath, type);
}

}