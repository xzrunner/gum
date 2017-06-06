#ifndef _GUM_STATISTICS_H_
#define _GUM_STATISTICS_H_

#include <CU_Singleton.h>

#include <fstream>

namespace gum
{

class Statistics
{
public:
	void SetEnable(bool enable) { m_enable = enable; }
	bool GetEnable() const { return m_enable; }

	void Update();
	void PrintGraph() const;
	void PrintFile();
	void Reset();

	void NoStatBegin();
	void NoStatEnd();

	void OptEnable(bool enable);

	void SetMem(float tot, float lua);

private:
	struct Memory
	{
		float tot;
		float tex;
		float lua;
		float loader;
		float other;
	};

private:
	bool m_enable;

	float m_tpf, m_tpf_smooth;

	float m_no_stat_begin;
	float m_no_stat_tot;

	Memory m_mem;

	bool m_opt_enable;

	std::ofstream m_fout;
	int m_fout_flush;

	SINGLETON_DECLARATION(Statistics);

}; // Statistics

}

#endif // _GUM_STATISTICS_H_