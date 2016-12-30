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
	: m_ortho_cam(new OrthoCamera())
	, m_p3d_cam(new Pseudo3DCamera())
	, m_width(0)
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
}

RenderContext::~RenderContext() 
{
	delete m_ortho_cam;
	delete m_p3d_cam;
}

void RenderContext::Init()
{

}

void RenderContext::OnSize(int w, int h)
{
	sl::SubjectMVP2::Instance()->NotifyProjection(w, h);

	m_p3d_cam->OnSize(w, h);
// 	sl::SubjectMVP3::Instance()->NotifyProjection(m_p3d_cam->GetProjectMat());

	RenderTarget::Instance()->OnSize(m_rc, w, h);

	m_width = w;
	m_height = h;
}

void RenderContext::SetCamera(float x, float y, float sx, float sy)
{
	sl::SubjectMVP2::Instance()->NotifyModelview(x, y, sx, sy);
	//sl::SubjectMVP3::Instance()->NotifyModelview(m_p3d_cam->GetModelViewMat());
}

}