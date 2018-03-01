#ifndef _GUM_RENDER_CONTEXT_H_
#define _GUM_RENDER_CONTEXT_H_

#include "gum/RenderTargetMgr.h"

#include <boost/noncopyable.hpp>

#include <guard/check.h>

#include <memory>

namespace ur { class RenderContext; }
namespace sl { class RenderContext; }
namespace pt2 { class RenderContext; }

namespace gum
{

class RenderContext : boost::noncopyable
{
public:
	RenderContext();

	void OnSize(int w, int h);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	ur::RenderContext& GetUrRc() const {
		GD_ASSERT(m_ur_rc, "null ur rc");
		return *m_ur_rc;
	}
	sl::RenderContext& GetSlRc() const {
		GD_ASSERT(m_sl_rc, "null sl rc");
		return *m_sl_rc;
	}
	pt2::RenderContext& GetPt2Rc() const {
		GD_ASSERT(m_pt2_rc, "null pt2 ctx");
		return *m_pt2_rc;
	}

	RenderTargetMgr& GetRTMgr() { return m_rt_mgr; }

	void Bind();
	void Unbind();

private:
	std::shared_ptr<ur::RenderContext>  m_ur_rc  = nullptr;
	std::shared_ptr<sl::RenderContext>  m_sl_rc  = nullptr;
	std::shared_ptr<pt2::RenderContext> m_pt2_rc = nullptr;
	
	RenderTargetMgr m_rt_mgr;

	int m_width, m_height;
	
}; // RenderContext

}

#endif // _GUM_RENDER_CONTEXT_H_
