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

IconSymLoader::IconSymLoader(s2::IconSymbol* sym, bool flatten)
	: m_sym(sym)
	, m_flatten(flatten)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

IconSymLoader::~IconSymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

void IconSymLoader::LoadJson(const std::string& filepath)
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

	std::string dir = FilepathHelper::Dir(filepath);

	// todo
}

void IconSymLoader::LoadBin(const simp::NodeIcon* node)
{
	if (!m_sym) {
		return;
	}

	std::vector<sm::vec2> vertices;
	ArrayLoader::Load(vertices, node->vertices, node->vertices_n, 16);

	s2::Icon* icon = NULL;
	switch (node->type)
	{
	case gum::ICON_RECT:
		{
			s2::StaticRectIcon* rect = new s2::StaticRectIcon();
			rect->SetRegion(sm::rect(vertices[0], vertices[1]));
			icon = rect;
		}
		break;
	case gum::ICON_QUAD:
		{
			s2::StaticQuadIcon* quad = new s2::StaticQuadIcon();
			quad->SetScreen(&vertices[0]);
			icon = quad;
		}
		break;
	case gum::ICON_CHANGED_RECT:
		{
			s2::DynamicRectIcon* drect = new s2::DynamicRectIcon();
			drect->SetRegion(
				sm::rect(vertices[0], vertices[1]),
				sm::rect(vertices[2], vertices[3]));
			icon = drect;
		}
		break;
	case gum::ICON_CHANGED_SECTOR:
		{
			s2::DynamicSectorIcon* sector = new s2::DynamicSectorIcon();
			sector->SetRegion(vertices[0].x, vertices[0].y);
			icon = sector;
		}
		break;
	}

	s2::Symbol* base = SymbolPool::Instance()->Fetch(node->base_id, m_flatten);
	assert(base->Type() == s2::SYM_IMAGE);
	icon->SetImage(VI_DOWNCASTING<s2::ImageSymbol*>(base));
	base->RemoveReference();

	if (icon) {
		m_sym->SetIcon(icon);
		icon->RemoveReference();
	}
}

}