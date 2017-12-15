#include "gum/StatTasks.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Shape2Shader.h>
#include <sprite2/RVG.h>
#include <rvg.h>

#include <algorithm>

namespace gum
{

CU_SINGLETON_DEFINITION(StatTasks)

StatTasks::StatTasks()
{
}

void StatTasks::Flush()
{
	auto& tasks = mt::TaskStat::Instance()->GetAllTasks();
//	for (auto& task : tasks)
	for (int i = 0, n = tasks.size(); i < n; ++i)
	{
		auto& task(tasks[i]);
		auto itr = m_tasks.find(task.thread_id);
		if (itr != m_tasks.end()) {
			itr->second.push_back(task);
		} else {
			CU_VEC<mt::TaskStat::Task> tasks;
			tasks.push_back(task);
			m_tasks.insert(std::make_pair(task.thread_id, tasks));
		}
	}

	for (auto& thread : m_tasks) {
		std::sort(thread.second.begin(), thread.second.end(), 
			[](const mt::TaskStat::Task& lhs, const mt::TaskStat::Task& rhs) -> bool
		{
			return lhs.begin < rhs.begin;
		});
	}	
}

void StatTasks::Print() const
{
	if (m_tasks.empty()) {
		return;
	}

	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	mgr->SetShader(sl::SHAPE2);
	sl::Shape2Shader* shader = static_cast<sl::Shape2Shader*>(mgr->GetShader());

	shader->SetColor(0xff0000ff);

	rvg_rect(0, 0, 100, 100, true);
}

}