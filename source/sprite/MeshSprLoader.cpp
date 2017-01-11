#include "MeshSprLoader.h"
#include "MeshSymLoader.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "MeshIO.h"

#include <sprite2/MeshSprite.h>
#include <sprite2/MeshSymbol.h>
#include <simp/NodeMeshSpr.h>
#include <simp/from_int.h>

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

	s2::MeshTransform& trans = m_spr->GetMeshTrans();
	MeshIO::Load(mesh_val, trans);
	trans.StoreToMesh(VI_DOWNCASTING<s2::MeshSymbol*>(m_spr->GetSymbol())->GetMesh());

	if (mesh_val.isMember("base_symbol")) {
		std::string base_path = FilepathHelper::Absolute(dir, mesh_val["base_symbol"].asString());
		s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(base_path);
		m_spr->SetBaseSym(base_sym);
		base_sym->RemoveReference();
	}
}

void MeshSprLoader::LoadBin(const simp::NodeMeshSpr* node)
{
	if (!m_spr) {
		return;
	}
	
	s2::MeshTransform& trans = m_spr->GetMeshTrans();

	std::map<sm::vec2, sm::vec2, sm::Vector2Cmp>& map = trans.GetMap();
	map.clear();
	int idx =0;
	for (int i = 0; i < node->n; ++i) 
	{
		sm::vec2 f, t;
		f.x = simp::int2float(int16_t(node->vertices[idx++]), 16);
		f.y = simp::int2float(int16_t(node->vertices[idx++]), 16);
		t.x = simp::int2float(int16_t(node->vertices[idx++]), 16);
		t.y = simp::int2float(int16_t(node->vertices[idx++]), 16);
		map.insert(std::make_pair(f, t));
	}
	trans.StoreToMesh(VI_DOWNCASTING<s2::MeshSymbol*>(m_spr->GetSymbol())->GetMesh());

	s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(node->base_id);
	m_spr->SetBaseSym(base_sym);
	base_sym->RemoveReference();
}

}