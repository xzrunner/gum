#include "AudioSymLoader.h"
#include "GUM_Audio.h"

#include <sprite2/AudioSymbol.h>
#include <uniaudio/AudioContext.h>
#include <uniaudio/Source.h>

namespace gum
{

AudioSymLoader::AudioSymLoader(const std::shared_ptr<s2::AudioSymbol>& sym)
	: m_sym(sym)
{
}

void AudioSymLoader::Load(const std::string& filepath)
{
	if (!m_sym) {
		return;
	}

	Audio* audio = Audio::Instance();
	if (!audio->IsEnable()) {
		return;
	}
	auto source = audio->GetContext()->CreateSource(filepath, true);
	m_sym->SetSource(source);
}

}