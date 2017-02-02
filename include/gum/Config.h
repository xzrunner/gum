#ifndef _GUM_CONFIG_H_
#define _GUM_CONFIG_H_

#include <CU_Singleton.h>

namespace gum
{

class Config
{
public:
	bool GetPreMulAlpha() const { return m_pre_mul_alpha; }
	void SetPreMulAlpha(bool pre_mul_alpha) { m_pre_mul_alpha = pre_mul_alpha; }

private:
	bool m_pre_mul_alpha;

	SINGLETON_DECLARATION(Config);

}; // Config

}

#endif // _GUM_CONFIG_H_