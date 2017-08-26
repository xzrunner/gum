#include "gum/ThreadPool.h"

#include <multitask/ThreadPool.h>
#include <multitask/TickThread.h>

// get_num_cores()
#ifdef _WIN32
#include <windows.h>
#elif __MACOSX
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

namespace gum
{

SINGLETON_DEFINITION(ThreadPool)

ThreadPool::ThreadPool()
{
	m_pool = new mt::ThreadPool();

	m_pool->SetMaxQueueSize(1024);

	int num_threads = GetNumCores();
	m_pool->Start(num_threads);

	m_tick = new mt::TickThread(m_pool);
}

ThreadPool::~ThreadPool()
{
	m_pool->Stop();
	delete m_pool;

	delete m_tick;
}

void ThreadPool::Run(mt::Task* task)
{
	m_pool->Run(task);
}

void ThreadPool::RegisterUpdateCB(void (*update)(void* arg), void* arg)
{
	m_tick->RegisterUpdateCB(update, arg);
}

void ThreadPool::UnregisterUpdateCB(void (*update)(void* arg))
{
	m_tick->UnregisterUpdateCB(update);
}

int ThreadPool::GetNumCores()
{
#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#elif __MACOSX
	int nm[2];
	size_t len = 4;
	uint32_t count;

	nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
	sysctl(nm, 2, &count, &len, NULL, 0);

	if(count < 1) {
		nm[1] = HW_NCPU;
		sysctl(nm, 2, &count, &len, NULL, 0);
		if(count < 1) { count = 1; }
	}
	return count;
#else
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

}