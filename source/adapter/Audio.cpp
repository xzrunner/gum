#include "gum/Audio.h"
#include "gum/ThreadPool.h"

#include <uniaudio/Callback.h>
#ifdef __ANDROID__
#include <uniaudio/opensl/AudioContext.h>
#else
#include <uniaudio/openal/AudioContext.h>
#endif // __ANDROID__

namespace gum
{

CU_SINGLETON_DEFINITION(Audio)

Audio::Audio() 
	: m_ctx(nullptr)
	, m_enable(true)
{
	InitCallback();
}

Audio::~Audio() 
{
	delete m_ctx;
}

void Audio::Initialize(void* arg1, void* arg2)
{
	if (m_ctx) {
		return;
	}

	try {
#ifdef __ANDROID__
		if (arg1 && arg2) {
			m_ctx = new ua::opensl::AudioContext((SLObjectItf)arg1, (SLObjectItf)arg2);
		} else {
			m_ctx = new ua::opensl::AudioContext();
		}
#else
		if (arg1 && arg2) {
			m_ctx = new ua::openal::AudioContext((ALCdevice*)arg1, (ALCcontext*)arg2);
		} else {
			m_ctx = new ua::openal::AudioContext();
		}
#endif // __ANDROID__
	} catch (std::exception&) {
		if (m_ctx) {
			delete m_ctx, m_ctx = nullptr;
		}
		m_enable = false;
	}
}

void Audio::Terminate()
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

void Audio::InitCallback()
{
	ua::Callback::API cb;
	cb.register_async_update = register_async_update;
	cb.unregister_async_update = unregister_async_update;

	ua::Callback::InitCallback(cb);
}

}
