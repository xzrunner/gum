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

static void state_change()
{
	sl::ShaderMgr::Instance()->Flush();
}

RenderContext::RenderContext() 
	: m_ortho_cam(new OrthoCamera())
	, m_p3d_cam(new Pseudo3DCamera())
	, m_width(0)
	, m_height(0)
{
	ur::gl::RenderContext::Callback cb;
	cb.state_change = state_change;
	m_rc = new ur::gl::RenderContext(cb);

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
}

void RenderContext::SetCamera(float x, float y, float sx, float sy)
{
	sl::SubjectMVP2::Instance()->NotifyModelview(x, y, sx, sy);
	//sl::SubjectMVP3::Instance()->NotifyModelview(m_p3d_cam->GetModelViewMat());
}

}