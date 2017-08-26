#ifndef _GUM_THREAD_POOL_H_
#define _GUM_THREAD_POOL_H_

#include <CU_Singleton.h>

namespace mt { class Task; class ThreadPool; }

namespace gum
{

class ThreadPool
{
public:
	void Submit(mt::Task* task);

private:
	mt::ThreadPool* m_pool;

	SINGLETON_DECLARATION(ThreadPool)

}; // ThreadPool

}

#endif // _GUM_THREAD_POOL_H_