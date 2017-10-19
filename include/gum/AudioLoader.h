#ifndef _GUM_AUDIO_LOADER_H_
#define _GUM_AUDIO_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

namespace ua { class AudioData; }

namespace gum
{

class AudioLoader : private cu::Uncopyable
{
public:
	AudioLoader(ua::AudioData* data);

	void LoadRaw(const CU_STR& filepath);

private:
	ua::AudioData* m_data;

}; // AudioLoader

}

#endif // _GUM_AUDIO_LOADER_H_