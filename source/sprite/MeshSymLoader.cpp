#include "MeshSymLoader.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "JsonSerializer.h"
#include "MeshIO.h"
#include "ArrayLoader.h"

#include <sprite2/MeshSymbol.h>
#include <sprite2/Mesh.h>
#include <sprite2/MeshTransform2.h>
#include <sprite2/NetworkMesh.h>
#include <sprite2/NetworkShape.h>
#include <sprite2/TrianglesMesh.h>
#include <sprite2/Skeleton2Mesh.h>
#include <simp/NodeMesh.h>
#include <simp/from_int.h>
#include <simp/NetworkMesh.h>
#include <simp/TrianglesMesh.h>
#include <simp/Skeleton2Mesh.h>
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

	s2::Mesh* mesh = NULL;
	switch (node->shape->Type())
	{
	case simp::MESH_NETWORK:
		mesh = LoadNetworkMesh(base_sym, static_cast<simp::NetworkMesh*>(node->shape));
		break;
	case simp::MESH_TRIANGLES:
		mesh = LoadTrianglesMesh(base_sym, static_cast<simp::TrianglesMesh*>(node->shape));
		break;
	case simp::MESH_SKELETON2:
		mesh = LoadSkeleton2Mesh(base_sym, static_cast<simp::Skeleton2Mesh*>(node->shape));
		break;
	}
	m_sym->SetMesh(mesh);
}

s2::Mesh* MeshSymLoader::LoadNetworkMesh(s2::Symbol* base_sym, simp::NetworkMesh* node)
{
	s2::NetworkMesh* mesh = new s2::NetworkMesh(base_sym);

	std::vector<sm::vec2> outer;
	ArrayLoader::Load(outer, node->outer, node->outer_n, 16);
	s2::NetworkShape* shape = new s2::NetworkShape(outer);

	std::vector<sm::vec2> inner;
	ArrayLoader::Load(inner, node->inner, node->inner_n, 16);
	shape->SetInnerVertices(inner);

	mesh->SetShape(shape);

	return mesh;
}

s2::Mesh* MeshSymLoader::LoadTrianglesMesh(s2::Symbol* base_sym, simp::TrianglesMesh* node)
{
	s2::TrianglesMesh* mesh = new s2::TrianglesMesh(base_sym);

	std::vector<sm::vec2> vertices;
	ArrayLoader::Load(vertices, node->vertices, node->vertices_n, 16);

	std::vector<sm::vec2> texcoords;
	ArrayLoader::Load(texcoords, node->texcoords, node->texcoords_n, 8192);

	std::vector<int> triangles;
	ArrayLoader::Load(triangles, node->triangle, node->triangle_n);

	mesh->SetData(vertices, texcoords, triangles);

	return mesh;
}

s2::Mesh* MeshSymLoader::LoadSkeleton2Mesh(s2::Symbol* base_sym, simp::Skeleton2Mesh* node)
{
	s2::Skeleton2Mesh* mesh = new s2::Skeleton2Mesh(base_sym);

	std::vector<s2::Skeleton2Mesh::Item> items;
	std::vector<s2::Skeleton2Mesh::Vertex> vertices;
	vertices.reserve(node->vertices_n);
	int ptr = 0;
	for (int i = 0; i < node->vertices_n; ++i)
	{
		int item_n = node->items_n[i];
		s2::Skeleton2Mesh::Vertex vdst;
		vdst.items.reserve(item_n);
		for (int j = 0, m = item_n; j < m; ++j) 
		{
			const simp::Skeleton2Mesh::Item& isrc = node->items[ptr++];
			s2::Skeleton2Mesh::Item idst;
			idst.joint    = isrc.joint;
			idst.vertex.x = simp::int2float(isrc.vx, 128);
			idst.vertex.y = simp::int2float(isrc.vy, 128);
			idst.offset.Set(0, 0);
			idst.weight = simp::int2float(isrc.weight, 4096);
			vdst.items.push_back(items.size());
			items.push_back(idst);
		}
		vertices.push_back(vdst);
	}
	
	std::vector<sm::vec2> texcoords;
	ArrayLoader::Load(texcoords, node->texcoords, node->texcoords_n, 8192);

	std::vector<int> triangles;
	ArrayLoader::Load(triangles, node->triangle, node->triangle_n);

	mesh->SetData(items, vertices, texcoords, triangles);

	return mesh;
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

	s2::MeshTransform2 trans;
	MeshIO::Load(val, trans, *mesh);
	trans.StoreToMesh(mesh);

	return mesh;
}

}