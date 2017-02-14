#include "MeshSprLoader.h"
#include "MeshSymLoader.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "MeshIO.h"

#include <sprite2/Mesh.h>
#include <sprite2/MeshSprite.h>
#include <sprite2/MeshSymbol.h>
#include <simp/NodeMeshSpr.h>
#include <simp/from_int.h>

#include <assert.h>

namespace gum
{

MeshSprLoader::MeshSprLoader(s2::MeshSprite* spr)
	: m_spr(spr)
{
	if (m_spr) {
		m_spr->AddReference();
	}
}

MeshSprLoader::~MeshSprLoader()
{
	if (m_spr) {
		m_spr->RemoveReference();
	}
}

void MeshSprLoader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_spr || !val.isMember("mesh")) {
		return;
	}

	const Json::Value& mesh_val = val["mesh"];

	pm::MeshTransform& trans = m_spr->GetMeshTrans();
	const s2::Mesh* mesh = VI_DOWNCASTING<s2::MeshSymbol*>(m_spr->GetSymbol())->GetMesh();
	MeshIO::Load(mesh_val, trans, *mesh);
	const_cast<s2::Mesh*>(mesh)->LoadFromTransform(trans);

	if (mesh_val.isMember("base_symbol")) {
		std::string base_path = FilepathHelper::Absolute(dir, mesh_val["base_symbol"].asString());
		s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(base_path);
		m_spr->SetBaseSym(base_sym);
		base_sym->RemoveReference();
	}
}

void MeshSprLoader::LoadBin(const simp::NodeMeshSpr* node)
{
	//if (!m_spr) {
	//	return;
	//}

	//s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(node->base_id);
	//m_spr->SetBaseSym(base_sym);
	//base_sym->RemoveReference();

	//assert(m_spr->GetSymbol()->Type() == s2::SYM_MESH);
	//s2::MeshSymbol* mesh_sym = VI_DOWNCASTING<s2::MeshSymbol*>(m_spr->GetSymbol());
	//s2::Mesh* mesh = mesh_sym->GetMesh();
	//if (!mesh) {
	//	return;
	//}

	//pm::MeshTransform& trans = m_spr->GetMeshTrans();
	//trans.Clear();

	//const std::vector<s2::MeshTriangle*>& tris = mesh->GetTriangles();
	//std::multimap<sm::vec2, int, sm::Vector2Cmp> map2idx;
	//int idx = 0;
	//for (int i = 0, n = tris.size(); i < n; ++i) {
	//	s2::MeshTriangle* tri = tris[i];
	//	for (int j = 0; j < 3; ++j) {
	//		map2idx.insert(std::make_pair(tri->nodes[j]->ori_xy, idx++));
	//	}
	//}

	//idx = 0;
	//for (int i = 0, n = node->n / 2; i < n; ++i) 
	//{
	//	sm::vec2 f, t;
	//	f.x = simp::int2float(int16_t(node->vertices[idx++]), 16);
	//	f.y = simp::int2float(int16_t(node->vertices[idx++]), 16);
	//	t.x = simp::int2float(int16_t(node->vertices[idx++]), 16);
	//	t.y = simp::int2float(int16_t(node->vertices[idx++]), 16);

	//	sm::vec2 pos = f - t;
	//	std::multimap<sm::vec2, int, sm::Vector2Cmp>::const_iterator 
	//		itr_begin = map2idx.lower_bound(f),
	//		itr_end = map2idx.upper_bound(f);
	//	assert(itr_begin != map2idx.end());
	//	std::multimap<sm::vec2, int, sm::Vector2Cmp>::const_iterator itr = itr_begin;
	//	for ( ; itr != itr_end; ++itr) {
	//		trans.Add(itr->second, pos);
	//		break;	// todo
	//	}
	//}
	//trans.StoreToMesh(mesh);
}

}