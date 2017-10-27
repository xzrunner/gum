#include "gum/IconSprLoader.h"

#include <sprite2/IconSprite.h>
#include <simp/NodeIconSpr.h>
#include <simp/from_int.h>

namespace gum
{

IconSprLoader::IconSprLoader(s2::IconSprite& spr)
	: m_spr(spr)
{
}

void IconSprLoader::LoadJson(const Json::Value& val, const CU_STR& dir)
{
}

void IconSprLoader::LoadBin(const simp::NodeIconSpr* node)
{
	m_spr.SetProcess(simp::int2float(node->process, 1024));
}

}