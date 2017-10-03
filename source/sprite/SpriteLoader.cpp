#include "SpriteLoader.h"
#include "SpriteFactory.h"

namespace gum
{

s2::SprPtr SpriteLoader::Create(const s2::SymPtr& sym) const
{
	return SpriteFactory::Instance()->Create(sym);
}

s2::SprPtr SpriteLoader::Create(const std::string& filepath) const
{
	return SpriteFactory::Instance()->Create(filepath);
}

s2::SprPtr SpriteLoader::Create(const Json::Value& val, const std::string& dir) const
{
	return SpriteFactory::Instance()->Create(val, dir);
}

}