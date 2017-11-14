#include "gum/AudioSprLoader.h"
#include "gum/Audio.h"

#include <sprite2/AudioSprite.h>
#include <uniaudio/AudioContext.h>
#include <uniaudio/Source.h>

namespace gum
{

AudioSprLoader::AudioSprLoader(s2::AudioSprite& spr)
	: m_spr(spr)
{
}

void AudioSprLoader::Load(const CU_STR& filepath)
{
	Audio* audio = Audio::Instance();
	if (!audio->IsEnable()) {
		return;
	}
	auto source = audio->GetContext()->CreateSource(filepath, true);
	m_spr.SetSource(source);
}

}