#ifndef _GUM_UPDATE_DTEX_C2_TASK_H_
#define _GUM_UPDATE_DTEX_C2_TASK_H_

#include <cu/cu_macro.h>
#include <SM_Rect.h>
#include <multitask/Task.h>

namespace gum
{

class UpdateDTexC2Task : public mt::Task
{
public:
	UpdateDTexC2Task(int thread_idx, uint32_t id, int tex_id, 
		int tex_w, int tex_h, const sm::i16_rect& region);

	virtual void Run() override;

	void Initialize(int thread_idx, uint32_t id, int tex_id, 
		int tex_w, int tex_h, const sm::i16_rect& region);

private:
	int m_thread_idx;

	uint32_t m_id;

	int m_tex_id;
	int m_tex_w, m_tex_h;

	sm::i16_rect m_region;

	friend class UpdateDTexC2TaskMgr;

}; // UpdateDTexC2Task

class UpdateDTexC2TaskMgr
{
public:
	UpdateDTexC2Task* Fetch(int thread_idx, uint32_t id, int tex_id, 
		int tex_w, int tex_h, const sm::i16_rect& region);

	void AddResult(UpdateDTexC2Task* task);

	void Flush();

private:
	std::vector<UpdateDTexC2Task*> m_freelists;
	std::vector<UpdateDTexC2Task*> m_results;

	CU_SINGLETON_DECLARATION(UpdateDTexC2TaskMgr)

}; // UpdateDTexC2TaskMgr

}

#endif // _GUM_UPDATE_DTEX_C2_TASK_H_