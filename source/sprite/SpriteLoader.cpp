#include "SpriteLoader.h"
#include "SpriteFactory.h"

namespace gum
{

s2::Sprite* SpriteLoader::Create(s2::Symbol* sym) const
{
	return SpriteFactory::Instance()->Create(sym);
}

s2::Sprite* SpriteLoader::Create(const std::string& filepath) const
{
	return SpriteFactory::Instance()->Create(filepath);
}

s2::Sprite* SpriteLoader::Create(const Json::Value& val, const std::string& dir, bool flatten) const
{
	return SpriteFactory::Instance()->Create(val, dir, flatten);
}

}