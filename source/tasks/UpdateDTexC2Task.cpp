#include "gum/UpdateDTexC2Task.h"
#include "gum/DTexC2Strategy.h"
#include "gum/ThreadPool.h"

#include <shaderlab/ShaderMgr.h>

#include <assert.h>

namespace gum
{

UpdateDTexC2Task::UpdateDTexC2Task(int thread_idx, uint32_t id, int tex_id, 
	                               int tex_w, int tex_h, const sm::i16_rect& region)
	: m_thread_idx(thread_idx)
	, m_id(id)
	, m_tex_id(tex_id)
	, m_tex_w(tex_w)
	, m_tex_h(tex_h)
	, m_region(region)
{
}

void UpdateDTexC2Task::Initialize(int thread_idx, uint32_t id, int tex_id, 
	                              int tex_w, int tex_h, const sm::i16_rect& region)
{
	m_thread_idx = thread_idx;

	m_id = id;

	m_tex_id = tex_id;
	m_tex_w = tex_w;
	m_tex_h = tex_h;

	m_region = region;
}

/************************************************************************/
/* class UpdateDTexC2TaskMgr                                            */
/************************************************************************/

CU_SINGLETON_DEFINITION(UpdateDTexC2TaskMgr)

UpdateDTexC2TaskMgr::UpdateDTexC2TaskMgr()
{	
	int thread_count = ThreadPool::Instance()->GetThreadCount();
	m_freelists.resize(thread_count);
	m_results.resize(thread_count);
}

void UpdateDTexC2TaskMgr::Add(int thread_idx, uint32_t id, int tex_id, 
	                          int tex_w, int tex_h, const sm::i16_rect& region)
{
	assert(thread_idx >= 0 && thread_idx < static_cast<int>(m_freelists.size()));
	UpdateDTexC2Task* task = m_freelists[thread_idx];
	if (!task) {
		task = new UpdateDTexC2Task(thread_idx, id, tex_id, tex_w, tex_h, region);
	} else {
		task = m_freelists[thread_idx];
		task->SetNext(nullptr);
		m_freelists[thread_idx] = static_cast<UpdateDTexC2Task*>(task->GetNext());
		task->Initialize(thread_idx, id, tex_id, tex_w, tex_h, region);
		task->RemoveReference();
	}

	assert(task->m_thread_idx >= 0 && task->m_thread_idx < static_cast<int>(m_results.size()));
	UpdateDTexC2Task* last = m_results[task->m_thread_idx];
	task->SetNext(last);
	m_results[task->m_thread_idx] = task;
	task->AddReference();
}

void UpdateDTexC2TaskMgr::Flush()
{
	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	sl::ShaderType type = mgr->GetShaderType();

	bool loaded = false;
	for (int i = 0, n = m_results.size(); i < n; ++i)
	{
		UpdateDTexC2Task* t = m_results[i];
		while (t)
		{
			UpdateDTexC2Task* next = static_cast<UpdateDTexC2Task*>(t->GetNext());
			if (DTexC2Strategy::Instance()->OnC2QueryFail(
				t->m_id, t->m_tex_id, t->m_tex_w, t->m_tex_h, t->m_region)) {
				loaded = true;
			}
			t->SetNext(m_freelists[i]);
			m_freelists[i] = t;
			t = next;
		}
		m_results[i] = nullptr;
	}
	if (loaded) {
		mgr->SetShader(type);
	}
}

}