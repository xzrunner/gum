#include "gum/RenderContext.h"
#include "gum/RenderTargetMgr.h"

#include <unirender/gl/RenderContext.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/SubjectMVP2.h>
#include <shaderlab/Blackboard.h>
#include <sprite2/Blackboard.h>

#include <stddef.h>

namespace gum
{

RenderContext::RenderContext() 
	: m_width(0)
	, m_height(0)
{
	ur::gl::RenderContext::Callback cb;
	cb.flush_shader = [&]() {
		m_shader_mgr->FlushShader();
	};
	cb.flush_render_shader = [&]() {
		m_shader_mgr->FlushRenderShader();
	};
#ifdef EASY_EDITOR
	m_rc = std::make_unique<ur::gl::RenderContext>(cb, 4096);
#else
	m_rc = std::make_unique<ur::gl::RenderContext>(cb, 1024);
#endif // S2_EDITOR

	m_shader_mgr = std::make_unique<sl::ShaderMgr>(*m_rc);
}

void RenderContext::OnSize(int w, int h)
{
	if (m_width == w && m_height == h) {
		return;
	}

	m_width = w;
	m_height = h;

	RenderTargetMgr::Instance()->OnSize(w, h);

	s2::Blackboard::Instance()->SetScreenSize(w, h);
}

void RenderContext::Bind()
{
	sl::Blackboard::Instance()->SetShaderMgr(m_shader_mgr);
}

}