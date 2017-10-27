#ifndef _GUM_AUDIO_SYM_LOADER_H_
#define _GUM_AUDIO_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <memory>

namespace s2 { class AudioSymbol; }

namespace gum
{

class AudioSymLoader : private cu::Uncopyable
{
public:
	AudioSymLoader(s2::AudioSymbol& sym);

	void Load(const CU_STR& filepath);

private:
	s2::AudioSymbol& m_sym;

}; // AudioSymLoader

}

#endif // _GUM_AUDIO_SYM_LOADER_H_