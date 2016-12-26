#ifndef _GUM_RENDER_TARGET_H_
#define _GUM_RENDER_TARGET_H_

#include <CU_Singleton.h>

namespace ur { class RenderTarget; class RenderContext; }

namespace gum
{

class RenderTarget
{
public:
	ur::RenderTarget* GetScreen0() { return m_screen0; }
	ur::RenderTarget* GetScreen1() { return m_screen1; }

	void OnSize(ur::RenderContext* rc, int w, int h);

private:
	ur::RenderTarget *m_screen0, *m_screen1;

	SINGLETON_DECLARATION(RenderTarget);

}; // RenderTarget

}

#endif // _GUM_RENDER_TARGET_H_