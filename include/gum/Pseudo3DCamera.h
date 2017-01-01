#ifndef _GUM_PSEUDO3D_CAMERA_H_
#define _GUM_PSEUDO3D_CAMERA_H_

#include "Camera.h"

#include <SM_Vector.h>
#include <SM_Matrix.h>

struct c25_camera;
struct sm_vec3;
union sm_mat4;

namespace gum
{

class Pseudo3DCamera : public Camera
{
public:
	Pseudo3DCamera();
	Pseudo3DCamera(const Pseudo3DCamera& cam);
	Pseudo3DCamera& operator = (const Pseudo3DCamera& cam);
	virtual ~Pseudo3DCamera();

	virtual CameraType Type() const { return CAM_PSEUDO3D; }
	virtual void OnSize(int width, int height);
	virtual void Reset();

	sm::vec2 TransPosScreenToProject(int x, int y, int width, int height) const;
	sm::vec2 TransPosProjectToScreen(const sm::vec3& proj, int width, int height) const;

	void Translate(const sm::vec3& offset);
	void Rotate(float da);

	float GetAngle() const;
	const sm_vec3* GetPos() const;

 	const sm_mat4* GetModelViewMat() const;
 	const sm_mat4* GetProjectMat() const;

private:
	void Init(const Pseudo3DCamera& cam);

	void UpdateRender() const;

private:
	c25_camera* m_cam;

}; // Pseudo3DCamera

}

#endif // _GUM_PSEUDO3D_CAMERA_H_
