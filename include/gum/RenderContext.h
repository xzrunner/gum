#ifndef _GUM_RENDER_CONTEXT_H_
#define _GUM_RENDER_CONTEXT_H_

#include <cu/cu_macro.h>

#include <stdint.h>

namespace ur { class RenderContext; }

namespace gum
{

class RenderContext
{
public:
	void OnSize(int w, int h);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	ur::RenderContext* GetImpl() { return m_rc; }

private:
	ur::RenderContext* m_rc;

	int m_width, m_height;

	CU_SINGLETON_DECLARATION(RenderContext)
	
}; // RenderContext

}

#endif // _GUM_RENDER_CONTEXT_H_
