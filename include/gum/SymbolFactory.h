#ifndef _GUM_SYMBOL_FACTORY_H_
#define _GUM_SYMBOL_FACTORY_H_

#include <cu/cu_macro.h>

#include <sprite2/SymType.h>
#include <sprite2/s2_typedef.h>

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
	s2::SymPtr Create(const std::string& filepath, int type = s2::SYM_UNKNOWN) const;

	/**
	 *  @brief
	 *    pack, binary res
	 */	
	s2::SymPtr Create(uint32_t id) const;
	s2::SymPtr Create(const std::string& pkg_name, const std::string& node_name) const;

private:
	mutable s2::SymPtr m_anchor_sym;

	CU_SINGLETON_DECLARATION(SymbolFactory);

}; // SymbolFactory

}

#endif // _GUM_SYMBOL_FACTORY_H_
