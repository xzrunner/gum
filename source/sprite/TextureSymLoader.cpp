#include "TextureSymLoader.h"
#include "FilepathHelper.h"
#include "ShapeLoader.h"
#include "SymbolFactory.h"

#include <sprite2/TextureSymbol.h>
#include <sprite2/PolygonShape.h>
#include <sprite2/ShapeSymbol.h>
#include <simp/NodeTexture.h>

#include <fstream>

namespace gum
{

TextureSymLoader::TextureSymLoader(const std::shared_ptr<s2::TextureSymbol>& sym)
	: m_sym(sym)
{
}

void TextureSymLoader::LoadJson(const CU_STR& filepath)
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

	if (!value.isMember("shapes")) {
		return;
	}

	CU_STR dir = FilepathHelper::Dir(filepath);

	for (int i = 0, n = value["shapes"].size(); i < n; ++i) {
		auto shape = ShapeLoader::LoadShape(value["shapes"][i], dir);
		auto poly_shape = static_cast<s2::PolygonShape*>(std::move(shape).release());
		m_sym->AddPolygon(std::unique_ptr<s2::PolygonShape>(poly_shape));
	}
}

void TextureSymLoader::LoadBin(const simp::NodeTexture* node)
{
	if (!m_sym) {
		return;
	}

	for (uint32_t i = 0; i < node->n; ++i) 
	{
		auto sym = SymbolFactory::Instance()->Create(node->polys[i]);
		if (sym)
		{
			auto shape_sym = S2_VI_PTR_DOWN_CAST<s2::ShapeSymbol>(sym);
			auto poly_shape = static_cast<s2::PolygonShape*>(std::move(shape_sym->GetShape()).release());
			m_sym->AddPolygon(std::unique_ptr<s2::PolygonShape>(poly_shape));
		}
	}
}

}