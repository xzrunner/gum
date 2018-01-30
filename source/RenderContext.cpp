#include "gum/RenderContext.h"
#include "gum/RenderTargetMgr.h"

#include <unirender/gl/RenderContext.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/SubjectMVP2.h>
#include <sprite2/Blackboard.h>

#include <stddef.h>

namespace gum
{

CU_SINGLETON_DEFINITION(RenderContext)

static void flush_shader()
{
	sl::ShaderMgr::Instance()->FlushShader();
}

static void flush_render_shader()
{
	sl::ShaderMgr::Instance()->FlushRenderShader();
}

RenderContext::RenderContext() 
	: m_width(0)
	, m_height(0)
{
	ur::gl::RenderContext::Callback cb;
	cb.flush_shader = flush_shader;
	cb.flush_render_shader = flush_render_shader;
#ifdef EASY_EDITOR
	m_rc = new ur::gl::RenderContext(cb, 4096);
#else
	m_rc = new ur::gl::RenderContext(cb, 1024);
#endif // S2_EDITOR

	sl::ShaderMgr::Instance()->SetContext(m_rc);
}

RenderContext::~RenderContext() 
{
	delete m_rc;
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

}