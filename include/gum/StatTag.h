#ifndef _GUM_STAT_TAG_H_
#define _GUM_STAT_TAG_H_

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>

#include <fstream>

namespace gum
{

class StatTag
{
public:
	void Enable(bool enable);
	bool IsEnable() const { return m_enable; }

	void Print(const CU_STR& type, const CU_STR& msg) const;
	void PrintScreen() const;

	void Flush();

private:
	void PrintFile() const;

private:
	bool m_enable;

	mutable CU_STR m_type;
	mutable CU_STR m_msg;

	mutable std::ofstream m_fout;

	CU_SINGLETON_DECLARATION(StatTag);

}; // StatTag
	
}

#endif // _GUM_STAT_TAG_H_