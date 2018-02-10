#include "gum/Cooking.h"
#include "gum/ThreadPool.h"

#include <sprite2/config.h>
#ifndef S2_DISABLE_DEFERRED
#include <cooking/Callback.h>
#include <cooking/DisplayOpAllocator.h>
#endif // S2_DISABLE_DEFERRED

namespace gum
{

static void
submit_task(mt::Task* task)
{
	ThreadPool::Instance()->Run(task);
}

void Cooking::Init()
{
#ifndef S2_DISABLE_DEFERRED
	cooking::Callback::Funs funs;
	funs.submit_task = submit_task;
	cooking::Callback::RegisterCallback(funs);

	cooking::DisplayOpAllocator::Instance()->Init(
		ThreadPool::Instance()->GetThreadCount());
#endif // S2_DISABLE_DEFERRED
}

}