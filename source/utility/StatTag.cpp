#include "gum/StatTag.h"
#include "gum/GUM_GTxt.h"

#include <shaderlab/ShaderMgr.h>
#include <sprite2/pre_defined.h>
#include S2_MAT_HEADER

#include <string>

#include <time.h>

namespace gum
{

CU_SINGLETON_DEFINITION(StatTag);

StatTag::StatTag()
	: m_enable(false)
{
}

void StatTag::Enable(bool enable)
{
	if (m_enable == enable) {
		return;
	}

	m_enable = enable;

	if (enable) 
	{
#ifdef __ANDROID__
		m_fout.open("/sdcard/lr_stat_tag.bin", std::ofstream::out | std::ofstream::binary);
#else
		m_fout.open("lr_stat_tag.bin", std::ofstream::out | std::ofstream::binary);	
#endif // __ANDROID__
	} 
	else 
	{
		m_fout.close();
	}
}

void StatTag::Print(const std::string& type, const std::string& msg) const
{
	if (!m_enable) {
		return;
	}

	m_type = type;
	m_msg = msg;

	PrintFile();
}

void StatTag::PrintScreen() const
{
	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	mgr->SetShader(sl::SPRITE2);

	static char buf[512];

	const int w = 960;
	const float top = 280;

	S2_MAT mt;
	mt.Translate(0, top);

	sprintf(buf, "%s", m_msg.c_str());
	GTxt::Instance()->Draw(mt, buf, w);	

	mgr->FlushShader();
}

void StatTag::Flush()
{
	if (m_enable) {
		m_fout.flush();
	}
}

void StatTag::PrintFile() const
{
	m_fout << "timestamp " << time(NULL) << ", type " << m_type << ", msg " << m_msg << '\n';

	static const int FLUSH_COUNT = 10;
	static int count = 0;
	++count;
	if (count == FLUSH_COUNT) {
		count = 0;
		m_fout.flush();
	}
}

}