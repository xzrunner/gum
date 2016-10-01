#ifndef _GUM_JOINT_POSE_IO_H_
#define _GUM_JOINT_POSE_IO_H_

#include <json/json.h>

namespace s2 { class JointPose; }

namespace gum
{

class JointPoseIO
{
public:
	static void Load(const Json::Value& val, s2::JointPose& pose);
	static void Store(Json::Value& val, const s2::JointPose& pose);

}; // JointPoseIO

}

#endif // _GUM_JOINT_POSE_IO_H_