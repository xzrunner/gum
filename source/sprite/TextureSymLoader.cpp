#include "TextureSymLoader.h"
#include "FilepathHelper.h"
#include "ShapeLoader.h"
#include "SymbolPool.h"

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

void TextureSymLoader::LoadJson(const std::string& filepath)
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

	std::string dir = FilepathHelper::Dir(filepath);

	for (int i = 0, n = value["shapes"].size(); i < n; ++i) {
		auto shape = ShapeLoader::LoadShape(value["shapes"][i], dir);
		std::unique_ptr<s2::PolygonShape> poly_shape(S2_VI_DOWN_CAST<s2::PolygonShape*>(shape.get()));
		m_sym->AddPolygon(std::move(poly_shape));
	}
}

void TextureSymLoader::LoadBin(const simp::NodeTexture* node)
{
	if (!m_sym) {
		return;
	}

	for (uint32_t i = 0; i < node->n; ++i) 
	{
		auto sym = SymbolPool::Instance()->Fetch(node->polys[i]);
		if (sym)
		{
			auto shape_sym = S2_VI_PTR_DOWN_CAST<s2::ShapeSymbol>(sym);
			std::unique_ptr<s2::PolygonShape> poly_shape(S2_VI_DOWN_CAST<s2::PolygonShape*>(shape_sym->GetShape().get()));
			m_sym->AddPolygon(poly_shape);
		}
	}
}

}