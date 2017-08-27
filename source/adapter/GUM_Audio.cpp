#include "GUM_Audio.h"
#include "ThreadPool.h"

#include <uniaudio/Callback.h>
#ifdef __ANDROID__
#include <uniaudio/opensl/AudioContext.h>
#else
#include <uniaudio/openal/AudioContext.h>
#endif // __ANDROID__

namespace gum
{

SINGLETON_DEFINITION(Audio)

Audio::Audio() 
{
	Init();

#ifdef __ANDROID__
	m_ctx = new ua::opensl::AudioContext();
#else
	m_ctx = new ua::openal::AudioContext();
#endif // __ANDROID__
}

Audio::~Audio() 
{
	delete m_ctx;
}

static void 
register_async_update(void (*update)(void* arg), void* arg)
{
	ThreadPool::Instance()->RegisterUpdateCB(update, arg);
}

static void 
unregister_async_update(void (*update)(void* arg))
{
	ThreadPool::Instance()->UnregisterUpdateCB(update);
}

void Audio::Init()
{
	ua::Callback::API cb;
	cb.register_async_update = register_async_update;
	cb.unregister_async_update = unregister_async_update;

	ua::Callback::InitCallback(cb);
}

}