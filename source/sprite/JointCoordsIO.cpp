#include "JointCoordsIO.h"

namespace gum
{

void JointCoordsIO::Load(const Json::Value& val, s2::LocalPose& local)
{
	local.trans = val["trans"].asDouble();
	local.rot = val["rot"].asDouble();
	local.scale = val["scale"].asDouble();
}

void JointCoordsIO::Store(Json::Value& val, const s2::LocalPose& local)
{
	val["trans"] = local.trans;
	val["rot"] = local.rot;
	val["scale"] = local.scale;
}

void JointCoordsIO::Load(const Json::Value& val, s2::WorldPose& world)
{
	world.pos.x = val["pos_x"].asDouble();
	world.pos.y = val["pos_y"].asDouble();
	world.angle = val["angle"].asDouble();
	world.scale = val["scale"].asDouble();
}

void JointCoordsIO::Store(Json::Value& val, const s2::WorldPose& world)
{
	val["pos_x"] = world.pos.x;
	val["pos_y"] = world.pos.y;
	val["angle"] = world.angle;
	val["scale"] = world.scale;
}

void JointCoordsIO::Load(const Json::Value& val, sm::vec2& offset)
{
	offset.x = val["x"].asDouble();
	offset.y = val["y"].asDouble();
}

void JointCoordsIO::Store(Json::Value& val, const sm::vec2& offset)
{
	val["x"] = offset.x;
	val["y"] = offset.y;
}

}