#include "gum/ShaderLab.h"
#include "gum/RenderContext.h"
#include "gum/ThreadPool.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/Callback.h>

namespace gum
{

CU_SINGLETON_DEFINITION(ShaderLab)

ShaderLab::ShaderLab() 
{
}

static void
submit_task(mt::Task* task)
{
	ThreadPool::Instance()->Run(task);
}

void ShaderLab::Init()
{
	sl::Callback::Funs funs;
	funs.submit_task = submit_task;
	sl::Callback::RegisterCallback(funs);
}

void ShaderLab::Update(float dt)
{
	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	sl::FilterShader* shader = static_cast<sl::FilterShader*>(mgr->GetShader(sl::FILTER));
	shader->UpdateTime(dt);
}

}