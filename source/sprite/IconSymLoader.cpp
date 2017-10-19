#include "IconSymLoader.h"
#include "IconType.h"
#include "FilepathHelper.h"
#include "SymbolPool.h"
#include "ArrayLoader.h"

#include <sprite2/IconSymbol.h>
#include <sprite2/StaticRectIcon.h>
#include <sprite2/StaticQuadIcon.h>
#include <sprite2/DynamicRectIcon.h>
#include <sprite2/DynamicSectorIcon.h>
#include <sprite2/ImageSymbol.h>
#include <simp/NodeIcon.h>
#include <simp/from_int.h>

#include <sprite2/SymType.h>

#include <fstream>

#include <assert.h>

namespace gum
{

IconSymLoader::IconSymLoader(const std::shared_ptr<s2::IconSymbol>& sym)
	: m_sym(sym)
{
}

void IconSymLoader::LoadJson(const CU_STR& filepath)
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

	CU_STR dir = FilepathHelper::Dir(filepath);

	// todo
}

void IconSymLoader::LoadBin(const simp::NodeIcon* node)
{
	if (!m_sym) {
		return;
	}

	CU_VEC<sm::vec2> vertices;
	ArrayLoader::Load(vertices, node->vertices, node->vertices_n, 16);

	std::unique_ptr<s2::Icon> icon;
	switch (node->type)
	{
	case gum::ICON_RECT:
		{
			auto rect = std::make_unique<s2::StaticRectIcon>();
			rect->SetRegion(sm::rect(vertices[0], vertices[1]));
			icon = std::move(rect);
		}
		break;
	case gum::ICON_QUAD:
		{
			auto quad = std::make_unique<s2::StaticQuadIcon>();
			quad->SetScreen(&vertices[0]);
			icon = std::move(quad);
		}
		break;
	case gum::ICON_CHANGED_RECT:
		{
			auto drect = std::make_unique<s2::DynamicRectIcon>();
			drect->SetRegion(
				sm::rect(vertices[0], vertices[1]),
				sm::rect(vertices[2], vertices[3]));
			icon = std::move(drect);
		}
		break;
	case gum::ICON_CHANGED_SECTOR:
		{
			auto sector = std::make_unique<s2::DynamicSectorIcon>();
			sector->SetRegion(vertices[0].x, vertices[0].y);
			icon = std::move(sector);
		}
		break;
	}

	auto base = SymbolPool::Instance()->Fetch(node->base_id);
	if (base) {
		assert(base->Type() == s2::SYM_IMAGE);
		icon->SetImage(S2_VI_PTR_DOWN_CAST<s2::ImageSymbol>(base));
	}

	if (icon) {
		m_sym->SetIcon(icon);
	}
}

}