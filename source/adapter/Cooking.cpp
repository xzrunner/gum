#include "gum/Cooking.h"
#include "gum/ThreadPool.h"

#include <cooking/Callback.h>

namespace gum
{

static void
submit_task(mt::Task* task)
{
	ThreadPool::Instance()->Run(task);
}

void Cooking::Init()
{
	cooking::Callback::Funs funs;
	funs.submit_task = submit_task;
	cooking::Callback::RegisterCallback(funs);
}

}