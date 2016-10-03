#ifndef _GUM_JOINT_LOADER_H_
#define _GUM_JOINT_LOADER_H_

#include <CU_RefCountObj.h>

namespace s2 { class Joint; class Sprite; class JointPose; }

namespace gum
{

class JointLoader : public cu::RefCountObj
{
public:
	virtual s2::Joint* Create(s2::Sprite* spr, const s2::JointPose& joint_pose) const;

}; // JointLoader

}

#endif // _GUM_JOINT_LOADER_H_