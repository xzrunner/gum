#include "gum/AudioContext.h"

#ifdef __ANDROID__
#include <uniaudio/opensl/AudioContext.h>
#else
#include <uniaudio/openal/AudioContext.h>
#endif // __ANDROID__

namespace gum
{

SINGLETON_DEFINITION(AudioContext)

AudioContext::AudioContext() 
{
#ifdef __ANDROID__
	m_ac = new ua::opensl::AudioContext();
#else
	m_ac = new ua::openal::AudioContext();
#endif // __ANDROID__
}

AudioContext::~AudioContext() 
{
	delete m_ac;
}

}
