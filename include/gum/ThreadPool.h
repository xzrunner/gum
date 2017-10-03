#ifndef _GUM_THREAD_POOL_H_
#define _GUM_THREAD_POOL_H_

#include <cu/cu_macro.h>

namespace mt { class Task; class ThreadPool; class TickThread; }

namespace gum
{

class ThreadPool
{
public:
	void Run(mt::Task* task);

	void RegisterUpdateCB(void (*update)(void* arg), void* arg);
	void UnregisterUpdateCB(void (*update)(void* arg));

	void Close();

private:
	mt::ThreadPool* m_pool;

	mt::TickThread* m_tick;

	CU_SINGLETON_DECLARATION(ThreadPool)

}; // ThreadPool

}

#endif // _GUM_THREAD_POOL_H_