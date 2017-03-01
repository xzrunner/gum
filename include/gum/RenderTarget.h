#ifndef _GUM_RENDER_TARGET_H_
#define _GUM_RENDER_TARGET_H_

#include <sprite2/S2_RenderTarget.h>

namespace gum
{

class RenderTarget : public s2::RenderTarget
{
public:
	RenderTarget(int width, int height);

	void Draw(float xmin = 0, float ymin = 0, float xmax = 1, float ymax = 1) const;

}; // RenderTarget

}

#endif // _GUM_RENDER_TARGET_H_