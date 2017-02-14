#ifndef _GUM_MESH_SYM_LOADER_H_
#define _GUM_MESH_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/json.h>

#include <string>

namespace s2 { class MeshSymbol; class Mesh; class Symbol; }
namespace simp { class NodeMesh; class PointsMesh; class TrianglesMesh; class Skin2Mesh; }

namespace gum
{

class MeshSymLoader : private cu::Uncopyable
{
public:
	MeshSymLoader(s2::MeshSymbol* sym);
	~MeshSymLoader();

	void LoadJson(const std::string& filepath);	
	void LoadBin(const simp::NodeMesh* node);

private:
	static s2::Mesh* LoadNetworkMesh(s2::Symbol* base_sym, simp::PointsMesh* mesh);
	static s2::Mesh* LoadTrianglesMesh(s2::Symbol* base_sym, simp::TrianglesMesh* mesh);
	static s2::Mesh* LoadSkeleton2Mesh(s2::Symbol* base_sym, simp::Skin2Mesh* mesh);

	static s2::Mesh* CreateNetworkMesh(const Json::Value& val, const s2::Symbol* base_sym);

private:
	s2::MeshSymbol* m_sym;

}; // MeshSymLoader

}

#endif // _GUM_MESH_SYM_LOADER_H_
