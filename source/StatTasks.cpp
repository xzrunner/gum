#include "gum/StatTasks.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Shape2Shader.h>
#include <sprite2/RVG.h>
#include <sprite2/RenderContext.h>
#include <sprite2/RenderCtxStack.h>
#include <rvg.h>

#include <algorithm>

namespace gum
{

CU_SINGLETON_DEFINITION(StatTasks)

StatTasks::StatTasks()
	: m_flush_flag(false)
{
}

void StatTasks::TriggerFlush() 
{ 
	m_flush_flag = true; 
}

void StatTasks::TryFlush() 
{
	if (m_flush_flag && m_tasks.empty()) {
		Flush();
	}
}

void StatTasks::Flush()
{
	mt::TaskStat* task_stat = mt::TaskStat::Instance();
	auto& tasks = task_stat->GetAllTasks();

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

	m_time_min = task_stat->GetTimeMin();
	m_time_max = task_stat->GetTimeMax();
}

void StatTasks::Print() const
{
	if (m_tasks.empty()) {
		return;
	}

	auto mgr = sl::ShaderMgr::Instance();
	mgr->SetShader(sl::SHAPE2);
	auto shader = static_cast<sl::Shape2Shader*>(mgr->GetShader());

	const s2::RenderContext* ctx = s2::RenderCtxStack::Instance()->Top();
	int tot_w = ctx->GetScreenWidth();
	int tot_h = ctx->GetScreenHeight();

	const int TILE_HEIGHT = 50;
	const int LINE_HEIGHT = 80;

	const float LENGTH = tot_w - 100;

	float start_x = -tot_w * 0.5 + 50,
		  start_y = tot_h * 0.5f - TILE_HEIGHT - 30;
	for (auto& list : m_tasks)
	{
		for (auto& task : list.second)
		{
			switch (task.type)
			{
			case 0:
				shader->SetColor(0xff0000ff);
				break;
			case 1:
				shader->SetColor(0xff00ffff);
				break;
			case 2:
				shader->SetColor(0xff00ff00);
				break;
			default:
				shader->SetColor(0xffffffff);
				break;
			}
			float x0 = start_x + LENGTH * (task.begin - m_time_min) / (m_time_max - m_time_min);
			float x1 = start_x + LENGTH * (task.end - m_time_min) / (m_time_max - m_time_min);
			rvg_rect(x0, start_y - TILE_HEIGHT, x1, start_y, true);
		}
		start_y -= LINE_HEIGHT;
	}
}

}