#include "StatFPS.h"

#include <dtex2/AsyncTask.h>
#include <glp_loop.h>

namespace gum
{

SINGLETON_DEFINITION(StatFPS);

StatFPS::StatFPS()
{
	m_enable = false;
	m_tot_fps = 0;
	m_tot_frames = 0;
	m_fps = 0;
	m_annealing = 0;
}

void StatFPS::Begin()
{
	m_enable = true;
	m_tot_fps = 0;
	m_tot_frames = 0;
	m_fps = 0;
	m_annealing = 0;
}

void StatFPS::End()
{
	m_enable = false;
	m_fps = m_tot_fps / m_tot_frames;
}

void StatFPS::Update()
{
	if (!m_enable) {
		return;
	}

	if (!dtex::AsyncTask::Instance()->IsEmpty()) {
		const int annealing = 100;
		m_annealing = annealing;
	}

	if (m_annealing != 0) {
		--m_annealing;
		return;
	}

	float dt = glp_get_dt();
	m_tot_fps += static_cast<int>(1000.0f / dt + 0.5f);
	++m_tot_frames;
}

}