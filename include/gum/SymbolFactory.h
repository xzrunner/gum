#ifndef _GUM_SYMBOL_FACTORY_H_
#define _GUM_SYMBOL_FACTORY_H_

#include <CU_Singleton.h>

#include <sprite2/SymType.h>

#include <string>

#include <stdint.h>

namespace s2 { class Symbol; }

namespace gum
{

class SymbolFactory
{
public:
	/**
	 *  @brief
	 *    raw, json res
	 */		
	s2::Symbol* Create(const std::string& filepath, int type = s2::SYM_UNKNOWN) const;

	/**
	 *  @brief
	 *    pack, binary res
	 */	
	s2::Symbol* Create(uint32_t id, bool flatten = false) const;
	s2::Symbol* Create(const std::string& pkg_name, const std::string& node_name) const;

	SINGLETON_DECLARATION(SymbolFactory);

private:
	mutable s2::Symbol* m_anchor_sym;

}; // SymbolFactory

}

#endif // _GUM_SYMBOL_FACTORY_H_
