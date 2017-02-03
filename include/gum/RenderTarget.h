#ifndef _GUM_RENDER_TARGET_H_
#define _GUM_RENDER_TARGET_H_

#include <sprite2/RenderTarget.h>

namespace gum
{

class RenderTarget : public s2::RenderTarget
{
public:
	RenderTarget(int width, int height);

	void Draw() const;

}; // RenderTarget

}

#endif // _GUM_RENDER_TARGET_H_