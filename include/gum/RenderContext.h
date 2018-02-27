#ifndef _GUM_RENDER_CONTEXT_H_
#define _GUM_RENDER_CONTEXT_H_

#include <boost/noncopyable.hpp>

#include <memory>

namespace ur { class RenderContext; }
namespace sl { class ShaderMgr; }

namespace gum
{

class RenderContext : boost::noncopyable
{
public:
	RenderContext();

	void OnSize(int w, int h);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	const std::shared_ptr<ur::RenderContext>& GetContext() const { return m_rc; }
	const std::shared_ptr<sl::ShaderMgr>& GetShaderMgr() const { return m_shader_mgr; }

	void Bind();

private:
	std::shared_ptr<ur::RenderContext> m_rc = nullptr;
	std::shared_ptr<sl::ShaderMgr>     m_shader_mgr = nullptr;
	
	int m_width, m_height;
	
}; // RenderContext

}

#endif // _GUM_RENDER_CONTEXT_H_
