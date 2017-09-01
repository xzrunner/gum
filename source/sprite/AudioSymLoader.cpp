#include "AudioSymLoader.h"
#include "GUM_Audio.h"

#include <sprite2/AudioSymbol.h>
#include <uniaudio/AudioContext.h>
#include <uniaudio/Source.h>

namespace gum
{

AudioSymLoader::AudioSymLoader(s2::AudioSymbol* sym)
	: m_sym(sym)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

AudioSymLoader::~AudioSymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

void AudioSymLoader::Load(const std::string& filepath)
{
	if (!m_sym) {
		return;
	}

	ua::AudioContext* ctx = gum::Audio::Instance()->GetContext();
	ua::Source* source = ctx->CreateSource(filepath, true);
	m_sym->SetSource(source);
	source->RemoveReference();
}

}