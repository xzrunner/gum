#ifndef _GUM_CAMERA_H_
#define _GUM_CAMERA_H_

#include "CameraType.h"

namespace gum
{

class Camera
{
public:
	virtual ~Camera() {}
	virtual CameraType Type() const = 0;
	virtual void OnSize(int width, int height) = 0;
	virtual void Reset() = 0;

}; // Camera

}

#endif // _GUM_CAMERA_H_
