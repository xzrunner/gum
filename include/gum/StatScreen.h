#ifndef _GUM_STAT_SCREEN_H_
#define _GUM_STAT_SCREEN_H_

#include <CU_Singleton.h>

#include <fstream>

namespace gum
{

class RenderTarget;

class StatScreen
{
public:
	void Enable(bool enable);
	bool IsEnable() const { return m_enable; }

	void Print(const RenderTarget* src);

	void Clear();

	void Flush();

	void DebugDraw() const;

private:
	static const int RT_EDGE = 4096;
	static const int GRID_EDGE = 256;
	static const int GRID_COUNT = RT_EDGE / GRID_EDGE;

private:
	bool m_enable;

	RenderTarget* m_rt;

	int m_x, m_y;

	mutable time_t m_timestamps[GRID_COUNT * GRID_COUNT];

	mutable std::ofstream m_fout;

	SINGLETON_DECLARATION(StatScreen);

}; // StatScreen

}

#endif // _GUM_STAT_SCREEN_H_