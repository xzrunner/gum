#ifndef _GUM_MESH_IO_H_
#define _GUM_MESH_IO_H_

#include <json/json.h>

namespace s2 { class MeshTransform; class MeshSkeleton; }

namespace gum
{

class MeshIO
{
public:
	// MeshTransform
	static void Load(const Json::Value& val, s2::MeshTransform& trans);
	static void Store(Json::Value& val, const s2::MeshTransform& trans);

	// MeshSkeleton
	static void Load(const Json::Value& val, s2::MeshSkeleton& sk);
	static void Store(Json::Value& val, const s2::MeshSkeleton& trans);

}; // MeshIO

}

#endif // _GUM_MESH_IO_H_