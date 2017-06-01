#ifndef _GUM_SYMBOL_POOL_H_
#define _GUM_SYMBOL_POOL_H_

#include <CU_Singleton.h>

#include <sprite2/SymType.h>

#include <string>
#include <map>

#include <stdint.h>

namespace s2 { class Symbol; }

namespace gum
{

class Image;

class SymbolPool
{
public:
	void GC();
	int Count() const { return m_path_cache.size() + m_id_cache.size(); }
	
	s2::Symbol* Fetch(const std::string& filepath, int type = s2::SYM_UNKNOWN);
	s2::Symbol* Fetch(const uint32_t id, bool flatten);

	void Clear();

	void PrintSymRef(uint32_t sym_id) const;
	void PrintImgRef(const Image* img) const;
	
private:
	std::map<std::string, s2::Symbol*> m_path_cache;
	std::map<uint32_t, s2::Symbol*>    m_id_cache;

	SINGLETON_DECLARATION(SymbolPool);

}; // SymbolPool

}

#endif // _GUM_SYMBOL_POOL_H_
