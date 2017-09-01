#include "GUM_Audio.h"
#include "gum/ThreadPool.h"

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
	: m_ctx(NULL)
	, m_enable(true)
{
	InitCallback();
}

Audio::~Audio() 
{
	delete m_ctx;
}

void Audio::InitContext(void* device, void* context)
{
#ifdef __ANDROID__
	m_ctx = new ua::opensl::AudioContext();
#else
	if (device && context) {
		m_ctx = new ua::openal::AudioContext((ALCdevice*)device, (ALCcontext*)context);
	} else {
		m_ctx = new ua::openal::AudioContext();
	}
#endif // __ANDROID__
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

void Audio::InitCallback()
{
	ua::Callback::API cb;
	cb.register_async_update = register_async_update;
	cb.unregister_async_update = unregister_async_update;

	ua::Callback::InitCallback(cb);
}

}
