#ifndef _GUM_JOINT_COORDS_IO_H_
#define _GUM_JOINT_COORDS_IO_H_

#include <sprite2/JointCoords.h>

#include <json/json.h>

namespace gum
{

class JointCoordsIO
{
public:
	static void Load(const Json::Value& val, s2::LocalPose& local);
	static void Store(Json::Value& val, const s2::LocalPose& local);

	static void Load(const Json::Value& val, s2::WorldPose& world);
	static void Store(Json::Value& val, const s2::WorldPose& world);

	static void Load(const Json::Value& val, sm::vec2& offset);
	static void Store(Json::Value& val, const sm::vec2& offset);

}; // JointCoordsIO

}

#endif // _GUM_JOINT_COORDS_IO_H_