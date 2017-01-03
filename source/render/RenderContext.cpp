#include "RenderContext.h"
#include "OrthoCamera.h"
#include "Pseudo3DCamera.h"
#include "RenderTarget.h"

#include <unirender/gl/RenderContext.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/SubjectMVP2.h>

#include <stddef.h>

namespace gum
{

SINGLETON_DEFINITION(RenderContext)

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
#endif // EASY_EDITOR

	sl::ShaderMgr::Instance()->SetContext(m_rc);

	m_cam = new OrthoCamera();
}

RenderContext::~RenderContext() 
{
	delete m_cam;
	delete m_rc;
}

void RenderContext::Init()
{
	// dummy
}

void RenderContext::OnSize(int w, int h)
{
	if (m_width == w && m_height == h) {
		return;
	}

	m_width = w;
	m_height = h;

	m_cam->OnSize(w, h);

	RenderTarget::Instance()->OnSize(m_rc, w, h);
}

}