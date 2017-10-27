#ifndef _GUM_MESH_SPR_LOADER_H_
#define _GUM_MESH_SPR_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <json/json.h>

namespace s2 { class MeshSprite; }
namespace simp { class NodeMeshSpr; }

namespace gum
{

class MeshSprLoader : private cu::Uncopyable
{
public:
	MeshSprLoader(s2::MeshSprite& spr);

	void LoadJson(const Json::Value& val, const CU_STR& dir);
	void LoadBin(const simp::NodeMeshSpr* node);

private:
	s2::MeshSprite& m_spr;

}; // MeshSprLoader

}

#endif // _GUM_MESH_SPR_LOADER_H_
