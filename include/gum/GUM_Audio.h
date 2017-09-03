#ifndef _GUM_AUDIO_H_
#define _GUM_AUDIO_H_

#include <CU_Singleton.h>

namespace ua { class AudioContext; }

namespace gum
{

class Audio
{
public:
	void InitContext(void* arg1, void* arg2);

	ua::AudioContext* GetContext() { return m_ctx; }

	bool IsEnable() const { return m_enable; }
	void SetEnable(bool enable) { m_enable = enable; }

private:
	void InitCallback();

private:
	ua::AudioContext* m_ctx;

	bool m_enable;

	SINGLETON_DECLARATION(Audio)

}; // Audio

}

#endif // _GUM_AUDIO_H_