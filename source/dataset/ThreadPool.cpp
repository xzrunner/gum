#include "gum/ThreadPool.h"

#include <multitask/ThreadPool.h>
#include <multitask/TickThread.h>

namespace gum
{

CU_SINGLETON_DEFINITION(ThreadPool)

ThreadPool::ThreadPool()
{
	m_pool = new mt::ThreadPool();

	m_pool->SetMaxQueueSize(1024);

	m_pool->Start(GetThreadCount());

	m_tick = new mt::TickThread(m_pool);
}

ThreadPool::~ThreadPool()
{
	Close();
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

void ThreadPool::Close()
{
	m_pool->Stop();
	delete m_pool;

	delete m_tick;
}

int ThreadPool::GetThreadCount() const
{
	return std::thread::hardware_concurrency();
}

int ThreadPool::QueryThreadIdx(std::thread::id id) const
{
	return m_pool->QueryThreadIdx(id);
}

}