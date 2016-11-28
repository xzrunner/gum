#include "SymbolPool.h"
#include "StringHelper.h"
#include "SymbolFactory.h"

#include <sprite2/S2_Symbol.h>

namespace gum
{

SINGLETON_DEFINITION(SymbolPool);

SymbolPool::SymbolPool()
{
}

void SymbolPool::GC()
{
	std::map<std::string, s2::Symbol*>::iterator itr0 = m_path_cache.begin();
	for ( ; itr0 != m_path_cache.end(); ) {
		if (itr0->second->GetRefCount() == 1) {
			itr0->second->RemoveReference();
			itr0 = m_path_cache.erase(itr0);
		} else {
			++itr0;
		}
	}

	std::map<uint32_t, s2::Symbol*>::iterator itr1 = m_id_cache.begin();
	for ( ; itr1 != m_id_cache.end(); ) {
		if (itr1->second->GetRefCount() == 1) {
			itr1->second->RemoveReference();
			itr1 = m_id_cache.erase(itr1);
		} else {
			++itr1;
		}
	}
}

s2::Symbol* SymbolPool::Fetch(const std::string& filepath, int type)
{
	std::string fixed_path = filepath;
	StringHelper::ToLower(fixed_path);
	std::map<std::string, s2::Symbol*>::const_iterator itr 
		= m_path_cache.find(fixed_path);
	if (itr != m_path_cache.end()) {
		itr->second->AddReference();
		return itr->second;
	}

	s2::Symbol* ret = SymbolFactory::Instance()->Create(filepath, type);
	if (ret) {
		ret->AddReference();
		m_path_cache.insert(std::make_pair(fixed_path, ret));
	}
	return ret;
}

s2::Symbol* SymbolPool::Fetch(const uint32_t id)
{
	std::map<uint32_t, s2::Symbol*>::const_iterator itr = m_id_cache.find(id);
	if (itr != m_id_cache.end()) {
		itr->second->AddReference();
		return itr->second;
	}

	s2::Symbol* ret = SymbolFactory::Instance()->Create(id);
	if (ret) {
		ret->AddReference();
		m_id_cache.insert(std::make_pair(id, ret));
	}
	return ret;
}

}