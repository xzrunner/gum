#ifndef _GUM_RENDER_TARGET_H_
#define _GUM_RENDER_TARGET_H_

#include <SM_Rect.h>
#include <painting2/RenderTarget.h>

#include <memory>

namespace pt2 { class WindowContext; }

namespace gum
{

class RenderTarget : public pt2::RenderTarget
{
public:
	RenderTarget(int width, int height);

	virtual void Bind() override;
	virtual void Unbind() override;

	void Draw(const sm::rect& src, const sm::rect& dst, int dst_w = 0, int dst_h = 0) const;

	void Clear();

private:
	std::shared_ptr<pt2::WindowContext> m_old_wc = nullptr;

}; // RenderTarget

}

#endif // _GUM_RENDER_TARGET_H_