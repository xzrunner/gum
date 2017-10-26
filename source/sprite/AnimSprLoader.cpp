#include "gum/AnimSprLoader.h"

#include <sprite2/AnimSprite.h>
#include <sprite2/UpdateParams.h>
#include <simp/from_int.h>
#include <simp/NodeAnimationSpr.h>

namespace gum
{

AnimSprLoader::AnimSprLoader(const std::shared_ptr<s2::AnimSprite>& spr)
	: m_spr(spr)
{
}

void AnimSprLoader::LoadJson(const Json::Value& val, const CU_STR& dir)
{
	if (!m_spr || !val.isMember("animation")) {
		return;
	}

	const Json::Value& anim_val = val["animation"];

	m_spr->SetLoop(anim_val["loop"].asBool());
	m_spr->SetInterval(static_cast<float>(anim_val["interval"].asDouble()));

	m_spr->SetFPS(anim_val["fps"].asInt());

	m_spr->SetStartRandom(s2::UpdateParams(), anim_val["start_random"].asBool());

	if (anim_val.isMember("active")) {
		m_spr->SetActive(anim_val["active"].asBool(), nullptr);
	}
}

void AnimSprLoader::LoadBin(const simp::NodeAnimationSpr* node)
{
	if (!m_spr) {
		return;
	}

	m_spr->SetLoop(simp::int2bool(node->loop));
	m_spr->SetInterval(simp::int2float(node->interval, 1024));

	m_spr->SetStartRandom(s2::UpdateParams(), simp::int2bool(node->start_random));

	m_spr->SetFPS(node->fps);
}

}