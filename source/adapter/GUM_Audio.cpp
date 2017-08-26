#include "GUM_Audio.h"
#include "ThreadPool.h"

#include <uniaudio/Callback.h>

namespace gum
{

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