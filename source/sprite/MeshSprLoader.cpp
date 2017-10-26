#include "gum/MeshSprLoader.h"
#include "gum/MeshSymLoader.h"
#include "gum/FilepathHelper.h"
#include "gum/SymbolPool.h"
#include "gum/MeshIO.h"

#include <sprite2/Mesh.h>
#include <sprite2/MeshSprite.h>
#include <sprite2/MeshSymbol.h>
#include <simp/NodeMeshSpr.h>
#include <simp/from_int.h>

#include <assert.h>

namespace gum
{

MeshSprLoader::MeshSprLoader(const std::shared_ptr<s2::MeshSprite>& spr)
	: m_spr(spr)
{
}

void MeshSprLoader::LoadJson(const Json::Value& val, const CU_STR& dir)
{
	if (!m_spr || !val.isMember("mesh")) {
		return;
	}

	const Json::Value& mesh_val = val["mesh"];

	pm::MeshTransform& trans = m_spr->GetMeshTrans();
	auto& mesh = S2_VI_PTR_DOWN_CAST<s2::MeshSymbol>(m_spr->GetSymbol())->GetMesh();
	MeshIO::Load(mesh_val, trans, *mesh);
	mesh->LoadFromTransform(trans);

	if (mesh_val.isMember("base_symbol")) {
		CU_STR base_path = FilepathHelper::Absolute(dir, mesh_val["base_symbol"].asString().c_str());
		auto base_sym = SymbolPool::Instance()->Fetch(base_path);
		if (base_sym) {
			m_spr->SetBaseSym(base_sym);
		}
	}
}

void MeshSprLoader::LoadBin(const simp::NodeMeshSpr* node)
{
	if (!m_spr) {
		return;
	}

	auto base_sym = SymbolPool::Instance()->Fetch(node->base_id);
	if (base_sym) {
		m_spr->SetBaseSym(base_sym);
	}

	assert(m_spr->GetSymbol()->Type() == s2::SYM_MESH);
	auto mesh_sym = S2_VI_PTR_DOWN_CAST<s2::MeshSymbol>(m_spr->GetSymbol());
	auto& mesh = mesh_sym->GetMesh();
	if (!mesh) {
		return;
	}

	CU_VEC<std::pair<int, sm::vec2> > trans;	
	int ptr = 0;
	for (uint32_t i = 0; i < node->n; ++i) 
	{
		int idx = node->trans[ptr++];
		float x = simp::int2float(int16_t(node->trans[ptr++]), 16),
			  y = simp::int2float(int16_t(node->trans[ptr++]), 16);
		trans.push_back(std::make_pair(idx, sm::vec2(x, y)));
	}
	m_spr->GetMeshTrans().SetTrans(trans);

	mesh->LoadFromTransform(m_spr->GetMeshTrans());
}

}