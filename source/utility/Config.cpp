#include "Config.h"

namespace gum
{

SINGLETON_DEFINITION(Config);

Config::Config()
{
	m_pre_mul_alpha = true;
}

}