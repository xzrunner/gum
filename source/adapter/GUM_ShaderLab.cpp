#include "GUM_ShaderLab.h"
#include "RenderContext.h"
#include "gum/ThreadPool.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Shape2Shader.h>
#include <shaderlab/Shape3Shader.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/Sprite3Shader.h>
#include <shaderlab/BlendShader.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/MaskShader.h>
#include <shaderlab/Model3Shader.h>
#include <shaderlab/Callback.h>

namespace gum
{

SINGLETON_DEFINITION(ShaderLab)

ShaderLab::ShaderLab() 
{
}

static void
submit_task(mt::Task* task)
{
	ThreadPool::Instance()->Run(task);
}

//void ShaderLab::Init()
//{
//	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
//	ur::RenderContext* rc = RenderContext::Instance()->GetImpl();
//	mgr->SetContext(rc);
//
//	mgr->CreateShader(sl::SHAPE2,	new sl::Shape2Shader(rc));
//	mgr->CreateShader(sl::SHAPE3,	new sl::Shape3Shader(rc));
//	mgr->CreateShader(sl::SPRITE2,	new sl::Sprite2Shader(rc));
//	mgr->CreateShader(sl::SPRITE3,	new sl::Sprite3Shader(rc));
//	mgr->CreateShader(sl::BLEND,	new sl::BlendShader(rc));
//	mgr->CreateShader(sl::FILTER,	new sl::FilterShader(rc));
//	mgr->CreateShader(sl::MASK,		new sl::MaskShader(rc));
//	mgr->CreateShader(sl::MODEL3,	new sl::Model3Shader(rc));
//}

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