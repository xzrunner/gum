#include "Anim2SprLoader.h"

#include <sprite2/Anim2Sprite.h>
#include <simp/from_int.h>
#include <simp/NodeAnimationSpr.h>

namespace gum
{

Anim2SprLoader::Anim2SprLoader(s2::Anim2Sprite* spr)
	: m_spr(spr)
{
	if (m_spr) {
		m_spr->AddReference();
	}
}

Anim2SprLoader::~Anim2SprLoader()
{
	if (m_spr) {
		m_spr->RemoveReference();
	}
}

void Anim2SprLoader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_spr || !val.isMember("anim2")) {
		return;
	}

	const Json::Value& anim_val = val["anim2"];
	m_spr->SetStaticTime(anim_val["static_time"].asInt());
}

}