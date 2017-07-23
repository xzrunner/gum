#ifndef _GUM_AUDIO_CONTEXT_H_
#define _GUM_AUDIO_CONTEXT_H_

#include <CU_Singleton.h>

#include <stdint.h>

namespace ua { class AudioContext; }

namespace gum
{

class AudioContext
{
public:
	void Init() {}

	ua::AudioContext* GetImpl() { return m_ac; }

private:
	ua::AudioContext* m_ac;

	SINGLETON_DECLARATION(AudioContext)
	
}; // AudioContext

}

#endif // _GUM_AUDIO_CONTEXT_H_
