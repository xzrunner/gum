#ifndef _GUM_AUDIO_SPR_LOADER_H_
#define _GUM_AUDIO_SPR_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <memory>

namespace s2 { class AudioSprite; }
namespace simp { class NodeAudioSpr; }

namespace gum
{

class AudioSprLoader : private cu::Uncopyable
{
public:
	AudioSprLoader(s2::AudioSprite& spr);

	void LoadBin(const CU_STR& filepath, const simp::NodeAudioSpr* node);

private:
	s2::AudioSprite& m_spr;

}; // AudioSprLoader

}

#endif // _GUM_AUDIO_SPR_LOADER_H_