#ifndef _GUM_CONFIG_H_
#define _GUM_CONFIG_H_

#include <cu/cu_macro.h>

namespace gum
{

class Config
{
public:
	bool GetPreMulAlpha() const { return m_pre_mul_alpha; }
	void SetPreMulAlpha(bool pre_mul_alpha) { m_pre_mul_alpha = pre_mul_alpha; }

	int GetLanguage() const { return m_lang; }
	void SetLanguage(int lang) { m_lang = lang; }

private:
	bool m_pre_mul_alpha;

	int m_lang;

	CU_SINGLETON_DECLARATION(Config);

}; // Config

}

#endif // _GUM_CONFIG_H_