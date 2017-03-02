#include "Scale9SymLoader.h"
#include "FilepathHelper.h"
#include "SpriteFactory.h"
#include "SymbolPool.h"
#include "SpriteLoader.h"

#include <sprite2/Scale9Symbol.h>
#include <sprite2/S2_Sprite.h>
#include <simp/NodeScale9.h>
#include <sm_const.h>

#include <fstream>

namespace gum
{

Scale9SymLoader::Scale9SymLoader(s2::Scale9Symbol* sym, const SpriteLoader* spr_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
{
	if (m_sym) {
		m_sym->AddReference();
	}
	if (m_spr_loader) {
		m_spr_loader->AddReference();
	} else {
		m_spr_loader = new SpriteLoader;
	}
}

Scale9SymLoader::~Scale9SymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_spr_loader->RemoveReference();
}

void Scale9SymLoader::LoadJson(const std::string& filepath)
{
	if (!m_sym) {
		return;
	}

	Json::Value val;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, val);
	fin.close();

	std::string dir = FilepathHelper::Dir(filepath);

	const Json::Value& spr_val = val["sprite"];

	s2::Sprite* grids[9];
	memset(grids, 0, sizeof(grids));

	int idx = 0;
	s2::SCALE9_TYPE type = s2::SCALE9_TYPE(val["type"].asInt());
	switch (type)
	{
	case s2::S9_9GRID:
		grids[s2::S9_DOWN_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_DOWN_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_DOWN_RIGHT]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_RIGHT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_RIGHT]		= m_spr_loader->Create(spr_val[idx++], dir);
		break;
	case s2::S9_9GRID_HOLLOW:
		grids[s2::S9_DOWN_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_DOWN_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_DOWN_RIGHT]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_RIGHT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_RIGHT]		= m_spr_loader->Create(spr_val[idx++], dir);
		break;
	case s2::S9_3GRID_HORI:
		grids[s2::S9_MID_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_RIGHT]		= m_spr_loader->Create(spr_val[idx++], dir);
		break;
	case s2::S9_3GRID_VERT:
		grids[s2::S9_DOWN_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		break;
	case s2::S9_6GRID_UPPER:
		grids[s2::S9_MID_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_MID_RIGHT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_LEFT]		= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_CENTER]	= m_spr_loader->Create(spr_val[idx++], dir);
		grids[s2::S9_TOP_RIGHT]		= m_spr_loader->Create(spr_val[idx++], dir);
		break;
	default:
		break;
	}

	int w = val["width"].asInt(),
		h = val["height"].asInt();
	m_sym->GetScale9().Build(type, w, h, grids, 0, 0, 0, 0);

	for (int i = 0; i < 9; ++i) {
		if (grids[i]) {
			grids[i]->RemoveReference();
		}
	}
}

void Scale9SymLoader::LoadBin(const simp::NodeScale9* node)
{
	s2::Sprite* grids[9];
	memset(grids, 0, sizeof(grids));

	const simp::NodeScale9::Grid* grid = NULL;

	int idx = 0;
	s2::SCALE9_TYPE type = s2::SCALE9_TYPE(node->type);
	switch (type)
	{
	case s2::S9_9GRID:
		grid = &node->grids[idx++];
		grids[s2::S9_DOWN_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_DOWN_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_DOWN_RIGHT]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_RIGHT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_RIGHT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		break;
	case s2::S9_9GRID_HOLLOW:
		grid = &node->grids[idx++];
		grids[s2::S9_DOWN_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_DOWN_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_DOWN_RIGHT]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_RIGHT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_RIGHT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		break;
	case s2::S9_3GRID_HORI:
		grid = &node->grids[idx++];
		grids[s2::S9_MID_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_RIGHT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		break;
	case s2::S9_3GRID_VERT:
		grid = &node->grids[idx++];
		grids[s2::S9_DOWN_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		break;
	case s2::S9_6GRID_UPPER:
		grid = &node->grids[idx++];
		grids[s2::S9_MID_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_MID_RIGHT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_LEFT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_CENTER]	= LoadSprite(grid->sym, grid->dir, grid->mirror);
		grid = &node->grids[idx++];
		grids[s2::S9_TOP_RIGHT]		= LoadSprite(grid->sym, grid->dir, grid->mirror);
		break;
	default:
		break;
	}

	const int DEFAULT_SIZE = 256;
	m_sym->GetScale9().Build(type, DEFAULT_SIZE, DEFAULT_SIZE, grids, 
		node->left, node->right, node->top, node->down);

	for (int i = 0; i < 9; ++i) {
		if (grids[i]) {
			grids[i]->RemoveReference();
		}
	}
}

s2::Sprite* Scale9SymLoader::LoadSprite(uint32_t sym_id, uint16_t dir, uint16_t mirror)
{
	s2::Symbol* sym = SymbolPool::Instance()->Fetch(sym_id);
	s2::Sprite* spr = SpriteFactory::Instance()->Create(sym);
	float angle = dir * SM_PI / 2;
	spr->SetAngle(angle);
	bool xmirror = mirror & 0x1,
		 ymirror = mirror & 0x2;
	sm::vec2 scale = spr->GetScale();
	if (xmirror) {
		scale.x = -fabs(scale.x);
	}
	if (ymirror) {
		scale.y = -fabs(scale.y);
	}
	spr->SetScale(scale);
	return spr;
}

}