#include "MeshSymLoader.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "JsonSerializer.h"
#include "MeshIO.h"

#include <sprite2/MeshSymbol.h>
#include <sprite2/Mesh.h>
#include <sprite2/MeshTransform.h>
#include <sprite2/NetworkMesh.h>
#include <sprite2/NetworkShape.h>
#include <simp/NodeMesh.h>
#include <simp/from_int.h>

#include <json/json.h>

#include <fstream>

namespace gum
{

MeshSymLoader::MeshSymLoader(s2::MeshSymbol* sym)
	: m_sym(sym)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

MeshSymLoader::~MeshSymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

void MeshSymLoader::LoadJson(const std::string& filepath)
{
	if (!m_sym) {
		return;
	}

	Json::Value value;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, value);
	fin.close();

	if (!value.isMember("base_symbol")) {
		return;
	}

	std::string dir = FilepathHelper::Dir(filepath);
	std::string base_path = FilepathHelper::Absolute(dir, value["base_symbol"].asString());
	s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(base_path);

	std::string type = value["type"].asString();
	s2::Mesh* mesh = NULL;
	if (type == "strip") {

	} else if (type == "network") {
		mesh = CreateNetworkMesh(value, base_sym);
	}

	m_sym->SetMesh(mesh);
	mesh->RemoveReference();
}

void MeshSymLoader::LoadBin(const simp::NodeMesh* node)
{
	if (!m_sym) {
		return;
	}

	s2::Symbol* base_sym = SymbolPool::Instance()->Fetch(node->base_id);
	s2::NetworkMesh* mesh = new s2::NetworkMesh(base_sym);

	std::vector<sm::vec2> outer;
	outer.reserve(node->outer_n);
	int idx = 0;
	for (int i = 0; i < node->outer_n; ++i) {
		float x = simp::int2float16x(int16_t(node->outer[idx++])),
			  y = simp::int2float16x(int16_t(node->outer[idx++]));
		outer.push_back(sm::vec2(x, y));
	}
	s2::NetworkShape* shape = new s2::NetworkShape(outer);

	std::vector<sm::vec2> inner;
	inner.reserve(node->inner_n);
	idx = 0;
	for (int i = 0; i < node->inner_n; ++i) {
		float x = simp::int2float16x(int16_t(node->inner[idx++])),
			  y = simp::int2float16x(int16_t(node->inner[idx++]));
		inner.push_back(sm::vec2(x, y));
	}
	shape->SetInnerVertices(inner);

	mesh->SetShape(shape);
	shape->RemoveReference();

	m_sym->SetMesh(mesh);
	mesh->RemoveReference();
}

s2::Mesh* MeshSymLoader::CreateNetworkMesh(const Json::Value& val, 
										   const s2::Symbol* sym)
{
	s2::NetworkMesh* mesh = new s2::NetworkMesh(sym);

	std::vector<sm::vec2> outline;
	JsonSerializer::Load(val["shape"]["outline"], outline);
	s2::NetworkShape* shape = new s2::NetworkShape(outline);
	
	std::vector<sm::vec2> inner;
	JsonSerializer::Load(val["shape"]["inner"], inner);
	shape->SetInnerVertices(inner);

	mesh->SetShape(shape);
	shape->RemoveReference();

	s2::MeshTransform trans;
	MeshIO::Load(val, trans);
	trans.StoreToMesh(mesh);

	return mesh;
}

}