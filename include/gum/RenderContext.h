#ifndef _GUM_RENDER_CONTEXT_H_
#define _GUM_RENDER_CONTEXT_H_

#include <boost/noncopyable.hpp>

#include <guard/check.h>

#include <memory>

namespace ur { class RenderContext; }
namespace sl { class RenderContext; }

namespace gum
{

class RenderContext : boost::noncopyable
{
public:
	RenderContext();

	void OnSize(int w, int h);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	ur::RenderContext& GetURRC() const {
		GD_ASSERT(m_ur_rc, "null ur rc");
		return *m_ur_rc;
	}
	sl::RenderContext& GetSLRC() const {
		GD_ASSERT(m_sl_rc, "null sl rc");
		return *m_sl_rc;
	}

	void Bind();

private:
	std::shared_ptr<ur::RenderContext> m_ur_rc = nullptr;
	std::shared_ptr<sl::RenderContext> m_sl_rc = nullptr;
	
	int m_width, m_height;
	
}; // RenderContext

}

#endif // _GUM_RENDER_CONTEXT_H_
