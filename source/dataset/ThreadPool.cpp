#include "gum/ThreadPool.h"

#include <multitask/ThreadPool.h>

namespace gum
{

SINGLETON_DEFINITION(ThreadPool)

ThreadPool::ThreadPool()
{
	m_pool = new mt::ThreadPool();
}

void ThreadPool::Submit(mt::Task* task)
{
	m_pool->Submit(task);
}

}