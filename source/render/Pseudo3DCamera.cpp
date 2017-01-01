#include "Pseudo3DCamera.h"
#include "RenderContext.h"

#include <c25_camera.h>
#include <sm_c_vector.h>

#include <stddef.h>

namespace gum
{

//static const float Z = -1000;
static const float Z = -250;
static const float ANGLE = -20;

Pseudo3DCamera::Pseudo3DCamera()
	: m_cam(NULL)
{
	int w = RenderContext::Instance()->GetWidth(),
		h = RenderContext::Instance()->GetHeight();
	OnSize(w, h);
}

Pseudo3DCamera::Pseudo3DCamera(const Pseudo3DCamera& cam)
{
	Init(cam);
}

Pseudo3DCamera& Pseudo3DCamera::operator = (const Pseudo3DCamera& cam)
{
	Init(cam);
	return *this;
}

Pseudo3DCamera::~Pseudo3DCamera()
{
	c25_cam_release(m_cam);
}

void Pseudo3DCamera::OnSize(int width, int height)
{
	s2::RenderContext* ctx = const_cast<s2::RenderContext*>(s2::RenderCtxStack::Instance()->Top());
	if (ctx) {
		ctx->SetProjection(width, height);
	}

	c25_cam_release(m_cam);

	sm_vec3 pos;
	pos.x = pos.y = 0;
	pos.z = Z;
	m_cam = c25_cam_create(&pos, ANGLE, (float)width / height);

	UpdateRender();
}

void Pseudo3DCamera::Reset()
{
	sm_vec3 pos;
	pos.x = pos.y = 0;
	pos.z = Z;
	c25_cam_set_pos(m_cam, &pos);

	c25_cam_set_angle(m_cam, ANGLE);

	UpdateRender();
}

sm::vec2 Pseudo3DCamera::TransPosScreenToProject(int x, int y, int width, int height) const
{
	sm_ivec2 screen;
	screen.x = x;
	screen.y = y;

	sm_vec2 world;
	c25_screen_to_world(m_cam, &world, &screen, width, height);

	return sm::vec2(world.x, world.y);
}

sm::vec2 Pseudo3DCamera::TransPosProjectToScreen(const sm::vec3& proj, int width, int height) const
{
	sm_vec3 world;
	world.x = proj.x;
	world.y = proj.y;
	world.z = proj.z;

	sm_ivec2 screen;
	c25_world_to_screen(m_cam, &screen, &world, width, height);

	return sm::vec2((float)screen.x, (float)screen.y);
}

void Pseudo3DCamera::Translate(const sm::vec3& offset)
{
	sm_vec3 vec;
	vec.x = offset.x;
	vec.y = offset.y;
	vec.z = offset.z;
	c25_cam_translate(m_cam, &vec);

	UpdateRender();
}

void Pseudo3DCamera::Rotate(float da)
{
	c25_cam_rotate(m_cam, da);

	UpdateRender();
}

float Pseudo3DCamera::GetAngle() const 
{
	return c25_cam_get_angle(m_cam);
}

const sm_vec3* Pseudo3DCamera::GetPos() const
{
	return c25_cam_get_pos(m_cam);
}

const sm_mat4* Pseudo3DCamera::GetModelViewMat() const
{
	return c25_cam_get_modelview_mat(m_cam);
}

const sm_mat4* Pseudo3DCamera::GetProjectMat() const
{
	return c25_cam_get_project_mat(m_cam);
}

void Pseudo3DCamera::Init(const Pseudo3DCamera& cam)
{
	const sm_vec3* pos = c25_cam_get_pos(cam.m_cam);
	float angle = c25_cam_get_angle(cam.m_cam);
	int w = RenderContext::Instance()->GetWidth(),
		h = RenderContext::Instance()->GetHeight();
	m_cam = c25_cam_create(pos, angle, (float)w / h);
}

void Pseudo3DCamera::UpdateRender() const
{
	float angle = c25_cam_get_angle(m_cam);

	// todo: should change s2::RenderContext, which will change sl mvp
// 	const sm_mat4* mat = c25_cam_get_modelview_mat(m_cam);
// 	sl::SubjectMVP3::Instance()->NotifyModelview(*(const sm::mat4*)mat);
}

}