#ifndef _GUM_STAT_FPS_H_
#define _GUM_STAT_FPS_H_

#include <cu/cu_macro.h>

namespace gum
{

class StatFPS
{
public:
	void Begin();
	void End();

	void Update();

	float GetFPS() const { return m_fps; }
	float GetFPSCurr() const { return m_tot_fps / m_tot_frames; }

private:
	bool  m_enable;
	float m_tot_fps;
	int   m_tot_frames;
	float m_fps;
	int   m_annealing;

	CU_SINGLETON_DECLARATION(StatFPS);

}; // StatFPS

}

#endif // _GUM_STAT_FPS_H_