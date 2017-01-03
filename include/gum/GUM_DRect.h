#ifndef _GUM_DRECT_H_
#define _GUM_DRECT_H_

#include <CU_Singleton.h>
#include <SM_Vector.h>
#include <sprite2/RenderContext.h>

namespace drect { class RenderTarget; }

namespace gum
{

class DRect
{
public:
	void Bind();
	void Unbind();

	bool Draw(const sm::vec2& offset, float scale);
	
	void DebugDraw() const;

public:
	static const int EXTEND = 256;

private:
	drect::RenderTarget* m_rt;

	s2::RenderContext m_ctx;

	bool m_draw_init;

	SINGLETON_DECLARATION(DRect)

}; // DRect

}

#endif // _GUM_DRECT_H_