#include "gum/ResPool.h"

namespace gum
{

std::unique_ptr<ResPool> ResPool::m_instance = nullptr;

ResPool& ResPool::Instance()
{
	if (!m_instance) {
		m_instance = std::make_unique<ResPool>();
	}
	return *m_instance;
}

ResPool::ResPool()
{
}

ResPool::~ResPool()
{
}

}