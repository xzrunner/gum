#include "SymbolPool.h"
#include "gum/StringHelper.h"
#include "SymbolFactory.h"
#include "GUM_ImageSymbol.h"
#include "Image.h"

#include <logger.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/SymbolVisitor.h>

namespace gum
{

CU_SINGLETON_DEFINITION(SymbolPool);

SymbolPool::SymbolPool()
{
}

void SymbolPool::Traverse(std::function<bool(const s2::SymPtr&)> func)
{
	for (auto& item : m_id_cache) {
		auto sym = item.second.lock();
		if (sym) {
			if (!func(sym)) {
				break;
			}
		}
	}
}

void SymbolPool::GC()
{
	{
		auto itr = m_path_cache.begin();
		for (; itr != m_path_cache.end(); )
		{
			if (itr->second.expired()) {
				itr = m_path_cache.erase(itr);
			}
			else {
				++itr;
			}
		}
	}
	{
		auto itr = m_id_cache.begin();
		for (; itr != m_id_cache.end(); )
		{
			if (itr->second.expired()) {
				itr = m_id_cache.erase(itr);
			}
			else {
				++itr;
			}
		}
	}
}

s2::SymPtr SymbolPool::Fetch(const CU_STR& filepath, int type)
{
	CU_STR fixed_path = filepath;
	StringHelper::ToLower(fixed_path);
	auto itr = m_path_cache.find(fixed_path);
	if (itr != m_path_cache.end()) {
		s2::SymPtr sym = itr->second.lock();
		if (sym) {
			return sym;
		}
	}

	auto ret = SymbolFactory::Instance()->Create(filepath, type);
	if (ret) {
		m_path_cache.insert(std::make_pair(fixed_path, ret));
	}
	return ret;
}

s2::SymPtr SymbolPool::Fetch(const uint32_t id)
{
	auto itr = m_id_cache.find(id);
	if (itr != m_id_cache.end()) 
	{
		s2::SymPtr ret = itr->second.lock();
		if (ret) {
			return ret;
		} else {
			m_id_cache.erase(itr);
		}
	}

	auto ret = SymbolFactory::Instance()->Create(id);
	if (ret) {
		m_id_cache.insert(std::make_pair(id, ret));
	}
	return ret;
}

void SymbolPool::Clear()
{
	m_path_cache.clear();
	m_id_cache.clear();
}

}