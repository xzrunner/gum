#include "P3dSprLoader.h"

#include <sprite2/Particle3dSprite.h>
#include <simp/NodeParticle3dSpr.h>
#include <simp/from_int.h>

namespace gum
{

P3dSprLoader::P3dSprLoader(s2::Particle3dSprite* spr)
	: m_spr(spr)
{
	if (m_spr) {
		m_spr->AddReference();
	}
}

P3dSprLoader::~P3dSprLoader()
{
	if (m_spr) {
		m_spr->RemoveReference();
	}
}

void P3dSprLoader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_spr || !val.isMember("particle3d")) {
		return;
	}

	const Json::Value& p_val = val["particle3d"];

	bool loop = false;
	if (p_val.isMember("loop")) {
		loop = p_val["loop"].asBool();
	}
	m_spr->SetLoop(loop);

	bool local_mode_draw = false;
	if (p_val.isMember("loop")) {
		local_mode_draw = p_val["local_mode_draw"].asBool();
	}
	m_spr->SetLocal(local_mode_draw);

	bool alone = false;
	if (p_val.isMember("alone")) {
		alone = p_val["alone"].asBool();
	}
	m_spr->SetAlone(alone);

	bool reuse = false;
	if (p_val.isMember("reuse")) {
		reuse = p_val["reuse"].asBool();
	}
	m_spr->SetReuse(reuse);

	if (p_val.isMember("start_radius")) {
		float start_radius = p_val["start_radius"].asDouble();
		m_spr->SetStartRadius(start_radius);
	}
}

void P3dSprLoader::LoadBin(const simp::NodeParticle3dSpr* node)
{
	if (!m_spr) {
		return;
	}

	m_spr->SetLoop(simp::int2bool(node->loop));
	m_spr->SetLocal(simp::int2bool(node->local));
	m_spr->SetAlone(simp::int2bool(node->alone));
	m_spr->SetReuse(simp::int2bool(node->reuse));

	m_spr->SetStartRadius(node->radius);
}

}