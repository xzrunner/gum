#ifndef _GUM_JOINT_LOADER_H_
#define _GUM_JOINT_LOADER_H_

#include <sprite2/s2_typedef.h>

namespace s2 { class Joint; class Sprite; class JointPose; }

namespace gum
{

class JointLoader
{
public:
	virtual std::shared_ptr<s2::Joint> Create(const s2::SprPtr& spr, const s2::JointPose& joint_pose) const;

}; // JointLoader

}

#endif // _GUM_JOINT_LOADER_H_