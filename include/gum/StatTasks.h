#ifndef _GUM_STAT_TASK_H_
#define _GUM_STAT_TASK_H_

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>
#include <multitask/TaskStat.h>

#include <thread>

namespace gum
{

class StatTasks
{
public:
	void Flush();

	void Print() const;

private:
	CU_MAP<std::thread::id, CU_VEC<mt::TaskStat::Task>> m_tasks;

	CU_SINGLETON_DECLARATION(StatTasks);

}; // StatTasks

}

#endif // _GUM_STAT_TASK_H_