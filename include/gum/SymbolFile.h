#ifndef _GUM_SYMBOL_FILE_H_
#define _GUM_SYMBOL_FILE_H_

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>

namespace gum
{

class SymbolFile
{
public:
	int Type(const CU_STR& filepath) const;

	const CU_STR& Tag(int type) const;

private:
	void Regist(int type, const CU_STR& tag);

private:
	CU_STR m_unknown_tag;

	CU_MAP<int, CU_STR> m_type2tag;
	CU_MAP<CU_STR, int> m_tag2type;

	CU_SINGLETON_DECLARATION(SymbolFile);

}; // SymbolFile

}

#endif // _GUM_SYMBOL_FILE_H_