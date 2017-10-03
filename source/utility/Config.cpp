#include <gum/Config.h>

namespace gum
{

CU_SINGLETON_DEFINITION(Config);

Config::Config()
{
	m_pre_mul_alpha = true;
}

}
