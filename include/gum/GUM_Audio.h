#ifndef _GUM_AUDIO_H_
#define _GUM_AUDIO_H_

#include <cu/cu_macro.h>

namespace ua { class AudioContext; }

namespace gum
{

class Audio
{
public:
	void Initialize(void* arg1, void* arg2);
	void Terminate();

	ua::AudioContext* GetContext() { return m_ctx; }

	bool IsEnable() const { return m_enable && m_ctx; }
	void SetEnable(bool enable) { m_enable = enable; }

private:
	void InitCallback();

private:
	ua::AudioContext* m_ctx;

	bool m_enable;

	CU_SINGLETON_DECLARATION(Audio)

}; // Audio

}

#endif // _GUM_AUDIO_H_