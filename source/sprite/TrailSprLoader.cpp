#include "TrailSprLoader.h"

#include <sprite2/TrailSprite.h>
#include <simp/NodeTrailSpr.h>
#include <simp/from_int.h>

namespace gum
{

TrailSprLoader::TrailSprLoader(s2::TrailSprite* spr)
	: m_spr(spr)
{
	if (m_spr) {
		m_spr->AddReference();
	}
}

TrailSprLoader::~TrailSprLoader()
{
	if (m_spr) {
		m_spr->RemoveReference();
	}
}

void TrailSprLoader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_spr || !val.isMember("trail")) {
		return;
	}

	const Json::Value& t_val = val["trail"];

	bool local = false;
	if (t_val.isMember("local")) {
		local = t_val["local"].asBool();
	}
	m_spr->SetLocal(local);
}

void TrailSprLoader::LoadBin(const simp::NodeTrailSpr* node)
{
	if (!m_spr) {
		return;
	}

	// todo
}

}