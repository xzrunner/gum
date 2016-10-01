#include "JointPoseIO.h"

#include <sprite2/JointPose.h>

namespace gum
{

void JointPoseIO::Load(const Json::Value& val, s2::JointPose& pose)
{
	pose.trans.x = val["trans_x"].asDouble();
	pose.trans.y = val["trans_y"].asDouble();
	pose.rot = val["rot"].asDouble();
	pose.scale = val["scale"].asDouble();
}

void JointPoseIO::Store(Json::Value& val, const s2::JointPose& pose)
{
	val["trans_x"] = pose.trans.x;
	val["trans_y"] = pose.trans.y;
	val["rot"] = pose.rot;
	val["scale"] = pose.scale;
}

}