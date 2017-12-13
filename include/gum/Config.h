#ifndef _GUM_CONFIG_H_
#define _GUM_CONFIG_H_

#include <cu/cu_macro.h>
#include <sprite2/TextTable.h>

namespace gum
{

class Config
{
public:
	bool GetPreMulAlpha() const { return m_pre_mul_alpha; }
	void SetPreMulAlpha(bool pre_mul_alpha) { m_pre_mul_alpha = pre_mul_alpha; }

	s2::TextTable::LanguageIdx GetLanguage() const { return m_lang; }
	void SetLanguage(s2::TextTable::LanguageIdx lang) { m_lang = lang; }

private:
	bool m_pre_mul_alpha;

	s2::TextTable::LanguageIdx m_lang;

	CU_SINGLETON_DECLARATION(Config);

}; // Config

}

#endif // _GUM_CONFIG_H_