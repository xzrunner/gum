#ifndef _GUM_STATISTICS_H_
#define _GUM_STATISTICS_H_

#include <cu/cu_macro.h>

#include <fstream>

#include <stdint.h>

namespace gum
{

class Statistics
{
public:
	void EnableGraph(bool enable);
	void EnableConsole(bool enable);
	void EnableFile(bool enable);

	bool IsGraphEnable() const;
	bool IsConsoleEnable() const;
	bool IsFileEnable() const;
	
	void Update();
	void Print();
	void Reset();

	void Flush();

	void NoStatBegin();
	void NoStatEnd();

	void OptEnable(bool enable);

	void SetMem(float tot, float lua);

private:
	void PrintScreen() const;
	void PrintConsole() const;
	void PrintFile() const;

private:
	static const uint32_t FLAG_PRINT_GRAPH    = 0x00000001;
	static const uint32_t FLAG_PRINT_CONSOLE  = 0x00000002;
	static const uint32_t FLAG_PRINT_FILE     = 0x00000004;

	struct Memory
	{
		float tot;
		float tex;
		float lua;
		float loader;
		float other;
	};

private:
	uint32_t m_flags;

	float m_tpf, m_tpf_smooth;

	uint32_t m_no_stat_begin;
	uint32_t m_no_stat_tot;

	Memory m_mem;

	bool m_opt_enable;

	mutable std::ofstream m_fout;

	CU_SINGLETON_DECLARATION(Statistics);

}; // Statistics

}

#endif // _GUM_STATISTICS_H_