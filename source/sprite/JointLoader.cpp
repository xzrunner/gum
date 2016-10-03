#include "JointLoader.h"

#include <sprite2/Joint.h>

namespace gum
{

s2::Joint* JointLoader::Create(s2::Sprite* spr, const s2::JointPose& joint_pose) const
{
	return new s2::Joint(spr, joint_pose);
}

}