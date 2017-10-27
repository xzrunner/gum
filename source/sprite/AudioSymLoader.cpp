#include "gum/AudioSymLoader.h"
#include "gum/Audio.h"

#include <sprite2/AudioSymbol.h>
#include <uniaudio/AudioContext.h>
#include <uniaudio/Source.h>

namespace gum
{

AudioSymLoader::AudioSymLoader(s2::AudioSymbol& sym)
	: m_sym(sym)
{
}

void AudioSymLoader::Load(const CU_STR& filepath)
{
	Audio* audio = Audio::Instance();
	if (!audio->IsEnable()) {
		return;
	}
	auto source = audio->GetContext()->CreateSource(filepath, true);
	m_sym.SetSource(source);
}

}