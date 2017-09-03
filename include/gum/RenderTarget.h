#ifndef _GUM_RENDER_TARGET_H_
#define _GUM_RENDER_TARGET_H_

#include <SM_Rect.h>
#include <sprite2/S2_RenderTarget.h>

namespace gum
{

class RenderTarget : public s2::RenderTarget
{
public:
	RenderTarget(int width, int height);

	virtual void Bind();
	virtual void Unbind();

	void Draw(const sm::rect& src, const sm::rect& dst, int dst_w = 0, int dst_h = 0) const;

	void Clear();

}; // RenderTarget

}

#endif // _GUM_RENDER_TARGET_H_