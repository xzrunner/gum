#include "MeshSprLoader.h"
#include "MeshSymLoader.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "MeshIO.h"

#include <sprite2/Mesh.h>
#include <sprite2/MeshSprite.h>
#include <sprite2/MeshSymbol.h>
#include <sprite2/MeshTriangle.h>
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

	s2::MeshTransform2& trans = m_spr->GetMeshTrans();
	const s2::Mesh* mesh = VI_DOWNCASTING<s2::MeshSymbol*>(m_spr->GetSymbol())->GetMesh();
	MeshIO::Load(mesh_val, trans, *mesh);
	trans.StoreToMesh(mesh);

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

	s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(node->base_id);
	m_spr->SetBaseSym(base_sym);
	base_sym->RemoveReference();

	assert(m_spr->GetSymbol()->Type() == s2::SYM_MESH);
	s2::MeshSymbol* mesh_sym = VI_DOWNCASTING<s2::MeshSymbol*>(m_spr->GetSymbol());
	s2::Mesh* mesh = mesh_sym->GetMesh();
	if (!mesh) {
		return;
	}

	s2::MeshTransform2& trans = m_spr->GetMeshTrans();
	trans.Clear();

	const std::vector<s2::MeshTriangle*>& tris = mesh->GetTriangles();
	int idx =0;
	for (int i = 0; i < node->n; ++i) 
	{
		sm::vec2 f, t;
		f.x = simp::int2float(int16_t(node->vertices[idx++]), 16);
		f.y = simp::int2float(int16_t(node->vertices[idx++]), 16);
		t.x = simp::int2float(int16_t(node->vertices[idx++]), 16);
		t.y = simp::int2float(int16_t(node->vertices[idx++]), 16);

		int idx = 0;
		sm::vec2 pos = t - f;
		for (int j = 0, m = tris.size(); j < m; ++j) {
			for (int k = 0; k < 3; ++k) {
				if (f == tris[j]->nodes[k]->ori_xy) {
					trans.Add(idx, pos);
				}
				++idx;
			}
		}
	}
	trans.StoreToMesh(mesh);
}

}