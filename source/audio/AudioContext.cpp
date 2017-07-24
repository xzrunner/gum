#include "gum/AudioContext.h"

#include <uniaudio/openal/AudioContext.h>

namespace gum
{

SINGLETON_DEFINITION(AudioContext)

AudioContext::AudioContext() 
{
	m_ac = new ua::openal::AudioContext();
}

AudioContext::~AudioContext() 
{
	delete m_ac;
}

}
