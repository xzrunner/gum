#ifndef _GUM_SYMBOL_FILE_H_
#define _GUM_SYMBOL_FILE_H_

#include <cu/cu_macro.h>

#include <string>
#include <map>

namespace gum
{

class SymbolFile
{
public:
	int Type(const std::string& filepath) const;

	const std::string& Tag(int type) const;

private:
	void Regist(int type, const std::string& tag);

public:
	static const std::string UNKNOWN_TAG;

private:
	std::map<int, std::string> m_type2tag;
	std::map<std::string, int> m_tag2type;

	CU_SINGLETON_DECLARATION(SymbolFile);

}; // SymbolFile

}

#endif // _GUM_SYMBOL_FILE_H_