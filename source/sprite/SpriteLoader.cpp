#include "SpriteLoader.h"
#include "SpriteFactory.h"

namespace gum
{

s2::Sprite* SpriteLoader::Load(const Json::Value& val, const std::string& dir) const
{
	return SpriteFactory::Instance()->Create(val, dir);
}

s2::Sprite* SpriteLoader::Load(const std::string& filepath) const
{
	return SpriteFactory::Instance()->Create(filepath);
}

}