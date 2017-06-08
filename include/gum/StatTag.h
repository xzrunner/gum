#ifndef _GUM_STAT_TAG_H_
#define _GUM_STAT_TAG_H_

#include <CU_Singleton.h>

#include <fstream>

namespace gum
{

class StatTag
{
public:
	void Enable(bool enable);
	bool IsEnable() const { return m_enable; }

	void Print(const std::string& type, const std::string& msg) const;
	void PrintScreen() const;

private:
	void PrintFile() const;

private:
	bool m_enable;

	mutable std::string m_type;
	mutable std::string m_msg;

	mutable std::ofstream m_fout;

	SINGLETON_DECLARATION(StatTag);

}; // StatTag
	
}

#endif // _GUM_STAT_TAG_H_