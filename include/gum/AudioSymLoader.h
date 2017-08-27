#ifndef _GUM_AUDIO_SYM_LOADER_H_
#define _GUM_AUDIO_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <string>

namespace s2 { class AudioSymbol; }

namespace gum
{

class AudioSymLoader : private cu::Uncopyable
{
public:
	AudioSymLoader(s2::AudioSymbol* sym);
	~AudioSymLoader();

	void Load(const std::string& filepath);

private:
	s2::AudioSymbol* m_sym;

}; // AudioSymLoader

}

#endif // _GUM_AUDIO_SYM_LOADER_H_