#include "MeshSprLoader.h"
#include "MeshSymLoader.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "MeshIO.h"

#include <sprite2/S2_Mesh.h>
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
		if (base_sym) {
			m_spr->SetBaseSym(base_sym);
			base_sym->RemoveReference();
		}
	}
}

void MeshSprLoader::LoadBin(const simp::NodeMeshSpr* node)
{
	if (!m_spr) {
		return;
	}

	s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(node->base_id);
	if (base_sym) {
		m_spr->SetBaseSym(base_sym);
		base_sym->RemoveReference();
	}

	assert(m_spr->GetSymbol()->Type() == s2::SYM_MESH);
	s2::MeshSymbol* mesh_sym = VI_DOWNCASTING<s2::MeshSymbol*>(m_spr->GetSymbol());
	s2::Mesh* mesh = mesh_sym->GetMesh();
	if (!mesh) {
		return;
	}

	std::vector<std::pair<int, sm::vec2> > trans;	
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