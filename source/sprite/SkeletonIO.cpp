#include "SkeletonIO.h"

#include <sprite2/Joint.h>
#include <sprite2/JointPose.h>
#include <sprite2/SkeletonPose.h>

namespace gum
{

/************************************************************************/
/* class JointPose                                                      */
/************************************************************************/

void SkeletonIO::Load(const Json::Value& val, s2::JointPose& pose)
{
	pose.trans.x = val["trans_x"].asDouble();
	pose.trans.y = val["trans_y"].asDouble();
	pose.rot = val["rot"].asDouble();
	pose.scale = val["scale"].asDouble();
}

void SkeletonIO::Store(Json::Value& val, const s2::JointPose& pose)
{
	val["trans_x"] = pose.trans.x;
	val["trans_y"] = pose.trans.y;
	val["rot"] = pose.rot;
	val["scale"] = pose.scale;
}

/************************************************************************/
/* class SkeletonPose                                                   */
/************************************************************************/

void SkeletonIO::Load(const Json::Value& val, s2::SkeletonPose& pose)
{
	std::vector<s2::JointPose> joints;
	joints.reserve(val["joint"].size());
	for (int i = 0, n = val["joint"].size(); i < n; ++i) {
		s2::JointPose joint;
		Load(val["joint"][i], joint);
		joints.push_back(joint);
	}
	pose.SetJointPose(joints);
}

void SkeletonIO::Store(Json::Value& val, const s2::SkeletonPose& pose)
{
	const std::vector<s2::JointPose>& joints = pose.GetJointPose();
	for (int i = 0, n = joints.size(); i < n; ++i) {
		Store(val["joint"][i], joints[i]);
	}
}

}