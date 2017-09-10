#include "gum/ThreadPool.h"

#include <multitask/ThreadPool.h>
#include <multitask/TickThread.h>

namespace gum
{

SINGLETON_DEFINITION(ThreadPool)

ThreadPool::ThreadPool()
{
	m_pool = new mt::ThreadPool();

	m_pool->SetMaxQueueSize(1024);

	unsigned const thread_count = std::thread::hardware_concurrency();
	m_pool->Start(thread_count);

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

}