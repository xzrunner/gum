#ifndef _GUM_AUDIO_LOADER_H_
#define _GUM_AUDIO_LOADER_H_

#include <CU_Uncopyable.h>

#include <string>

namespace ua { class AudioData; }

namespace gum
{

class AudioLoader : private cu::Uncopyable
{
public:
	AudioLoader(ua::AudioData* data);

	void LoadRaw(const std::string& filepath);

private:
	ua::AudioData* m_data;

}; // AudioLoader

}

#endif // _GUM_AUDIO_LOADER_H_