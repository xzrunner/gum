#ifndef _GUM_AUDIO_H_
#define _GUM_AUDIO_H_

#include <CU_Singleton.h>

namespace ua { class AudioContext; }

namespace gum
{

class Audio
{
public:
	ua::AudioContext* GetContext() { return m_ctx; }

private:
	void Init();

private:
	ua::AudioContext* m_ctx;

	SINGLETON_DECLARATION(Audio)

}; // Audio

}

#endif // _GUM_AUDIO_H_