#ifndef _GUM_SYMBOL_POOL_H_
#define _GUM_SYMBOL_POOL_H_

#include <cu/cu_macro.h>

#include <sprite2/SymType.h>
#include <sprite2/s2_typedef.h>

#include <string>
#include <map>
#include <functional>

#include <stdint.h>

namespace s2 { class Symbol; }

namespace gum
{

class Image;

class SymbolPool
{
public:
	void Traverse(std::function<bool(const s2::SymPtr&)> func);

	void GC();
	int Count() const { return m_path_cache.size() + m_id_cache.size(); }
	
	s2::SymPtr Fetch(const std::string& filepath, int type = s2::SYM_UNKNOWN);
	s2::SymPtr Fetch(const uint32_t id);

	void Clear();

private:
	std::map<std::string, std::weak_ptr<s2::Symbol>> m_path_cache;
	std::map<uint32_t, std::weak_ptr<s2::Symbol>>    m_id_cache;

	CU_SINGLETON_DECLARATION(SymbolPool);

}; // SymbolPool

}

#endif // _GUM_SYMBOL_POOL_H_
