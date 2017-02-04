#ifndef _GUM_RENDER_CONTEXT_H_
#define _GUM_RENDER_CONTEXT_H_

#include <CU_Singleton.h>

#include <stdint.h>

namespace ur { class RenderContext; }
namespace s2 { class Camera; }

namespace gum
{

class RenderContext
{
public:
	void Init();

	void OnSize(int w, int h);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	ur::RenderContext* GetImpl() { return m_rc; }

private:
	ur::RenderContext* m_rc;

	int m_width, m_height;

	s2::Camera* m_cam;

	SINGLETON_DECLARATION(RenderContext)
	
}; // RenderContext

}

#endif // _GUM_RENDER_CONTEXT_H_
