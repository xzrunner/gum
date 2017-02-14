#include "MeshSymLoader.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "JsonSerializer.h"
#include "MeshIO.h"
#include "ArrayLoader.h"

#include <simp/NodeMesh.h>
#include <simp/from_int.h>
#include <simp/PointsMesh.h>
#include <simp/TrianglesMesh.h>
#include <simp/Skin2Mesh.h>
#include <simp/from_int.h>
#include <polymesh/PointsMesh.h>
#include <polymesh/TrianglesMesh.h>
#include <polymesh/Skin2Mesh.h>
#include <polymesh/MeshTransform.h>
#include <sprite2/MeshSymbol.h>
#include <sprite2/Mesh.h>

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
	case simp::MESH_POINTS:
		mesh = LoadNetworkMesh(base_sym, static_cast<simp::PointsMesh*>(node->shape));
		break;
	case simp::MESH_TRIANGLES:
		mesh = LoadTrianglesMesh(base_sym, static_cast<simp::TrianglesMesh*>(node->shape));
		break;
	case simp::MESH_SKIN2:
		mesh = LoadSkeleton2Mesh(base_sym, static_cast<simp::Skin2Mesh*>(node->shape));
		break;
	}
	m_sym->SetMesh(mesh);
}

s2::Mesh* MeshSymLoader::LoadNetworkMesh(s2::Symbol* base_sym, simp::PointsMesh* node)
{
	s2::Mesh* s2_mesh = new s2::Mesh(base_sym);
	
	std::vector<sm::vec2> outline;
	ArrayLoader::Load(outline, node->outline, node->outline_n, 16);

	std::vector<sm::vec2> points;
	ArrayLoader::Load(points, node->points, node->points_n, 16);

	sm::rect r = base_sym->GetBounding();
	pm::Mesh* pm_mesh = new pm::PointsMesh(outline, points, r.Width(), r.Height());
	s2_mesh->SetMesh(pm_mesh);
	
	return s2_mesh;
}

s2::Mesh* MeshSymLoader::LoadTrianglesMesh(s2::Symbol* base_sym, simp::TrianglesMesh* node)
{
	s2::Mesh* s2_mesh = new s2::Mesh(base_sym);

	std::vector<sm::vec2> vertices;
	ArrayLoader::Load(vertices, node->vertices, node->vertices_n, 16);

	std::vector<sm::vec2> texcoords;
	ArrayLoader::Load(texcoords, node->texcoords, node->texcoords_n, 8192);

	std::vector<int> triangles;
	ArrayLoader::Load(triangles, node->triangle, node->triangle_n);

	pm::Mesh* pm_mesh = new pm::TrianglesMesh(vertices, texcoords, triangles);
	s2_mesh->SetMesh(pm_mesh);

	return s2_mesh;
}

s2::Mesh* MeshSymLoader::LoadSkeleton2Mesh(s2::Symbol* base_sym, simp::Skin2Mesh* node)
{
	s2::Mesh* s2_mesh = new s2::Mesh(base_sym);

	std::vector<pm::Skin2Joint> joints;
	int joints_n = 0;
	for (int i = 0; i < node->vertices_n; ++i) {
		joints_n += node->joints_n[i];
	}
	joints.reserve(joints_n);
	for (int i = 0; i < joints_n; ++i) 
	{
		const simp::Skin2Mesh::Joint& src = node->joints[i];
		pm::Skin2Joint dst;
		dst.joint = src.joint;
		dst.vertex.x = simp::int2float(src.vx, 128);
		dst.vertex.y = simp::int2float(src.vy, 128);
		dst.offset.Set(0, 0);
		dst.weight = simp::int2float(src.weight, 4096);
		joints.push_back(dst);
	}

	std::vector<int> vertices;
	vertices.reserve(node->vertices_n);
	for (int i = 0; i < node->vertices_n; ++i) {
		vertices.push_back(node->joints_n[i]);
	}

	std::vector<sm::vec2> texcoords;
	ArrayLoader::Load(texcoords, node->texcoords, node->texcoords_n, 8192);

	std::vector<int> triangles;
	ArrayLoader::Load(triangles, node->triangle, node->triangle_n);

	pm::Mesh* pm_mesh = new pm::Skin2Mesh(joints, vertices, texcoords, triangles);
	s2_mesh->SetMesh(pm_mesh);

	return s2_mesh;
}

s2::Mesh* MeshSymLoader::CreateNetworkMesh(const Json::Value& val, const s2::Symbol* base_sym)
{
	s2::Mesh* s2_mesh = new s2::Mesh(base_sym);

	std::vector<sm::vec2> outline;
	JsonSerializer::Load(val["shape"]["outline"], outline);

	std::vector<sm::vec2> points;
	JsonSerializer::Load(val["shape"]["inner"], points);

	sm::rect r = base_sym->GetBounding();
	pm::Mesh* pm_mesh = new pm::PointsMesh(outline, points, r.Width(), r.Height());

	pm::MeshTransform trans;
	MeshIO::Load(val, trans, *s2_mesh);
	pm_mesh->LoadFromTransform(trans);

	s2_mesh->SetMesh(pm_mesh);

	return s2_mesh;
}

}