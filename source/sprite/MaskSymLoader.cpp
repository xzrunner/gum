#include "gum/MaskSymLoader.h"
#include "gum/FilepathHelper.h"
#include "gum/SpriteFactory.h"

#include <sprite2/MaskSymbol.h>
#include <sprite2/Sprite.h>
#include <simp/NodeMask.h>

#include <json/json.h>

#include <fstream>

namespace gum
{

MaskSymLoader::MaskSymLoader(s2::MaskSymbol& sym)
	: m_sym(sym)
{
}

void MaskSymLoader::LoadJson(const CU_STR& filepath)
{
	Json::Value value;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, value);
	fin.close();

	if (!value.isMember("base") || !value.isMember("mask")) {
		return;
	}

	CU_STR dir = FilepathHelper::Dir(filepath);

	CU_STR base_path = FilepathHelper::Absolute(dir, value["base"]["filepath"].asString().c_str());
	auto base = SpriteFactory::Instance()->Create(base_path);
	if (base) {
		m_sym.SetBase(base);
	}

	CU_STR mask_path = FilepathHelper::Absolute(dir, value["mask"]["filepath"].asString().c_str());
	auto mask = SpriteFactory::Instance()->Create(mask_path);
	if (mask) {
		m_sym.SetMask(mask);
	}
}

void MaskSymLoader::LoadBin(const simp::NodeMask* node)
{
	auto base = SpriteFactory::Instance()->Create(node->base_id);
	if (base) {
		m_sym.SetBase(base);
	}

	auto mask = SpriteFactory::Instance()->Create(node->mask_id);
	if (mask) {
		m_sym.SetMask(mask);
	}
}

}