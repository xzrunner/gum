#ifndef _GUM_STAT_FPS_H_
#define _GUM_STAT_FPS_H_

#include <CU_Singleton.h>

namespace gum
{

class StatFPS
{
public:
	void Begin();
	void End();

	void Update();

	float GetFPS() const { return m_fps; }
	float GetFPSCurr() const {
		return static_cast<float>(m_tot_fps / m_tot_frames);
	}

private:
	bool  m_enable;
	float m_tot_fps;
	int   m_tot_frames;
	float m_fps;
	int   m_annealing;

	SINGLETON_DECLARATION(StatFPS);

}; // StatFPS

}

#endif // _GUM_STAT_FPS_H_