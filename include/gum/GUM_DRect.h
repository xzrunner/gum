#ifndef _GUM_DRECT_H_
#define _GUM_DRECT_H_

#include <CU_Singleton.h>

namespace drect { class RenderTarget; }

namespace gum
{

class DRect
{
public:
	void Bind();
	void Unbind();
	
	void DebugDraw() const;

private:
	drect::RenderTarget* m_rt;

	SINGLETON_DECLARATION(DRect)

}; // DRect

}

#endif // _GUM_DRECT_H_