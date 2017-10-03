#ifndef _GUM_SPRITE_LOADER_H_
#define _GUM_SPRITE_LOADER_H_

#include <sprite2/s2_typedef.h>

#include <json/json.h>

namespace gum
{

class SpriteLoader
{
public:
	virtual s2::SprPtr Create(const s2::SymPtr& sym) const;
	virtual s2::SprPtr Create(const std::string& filepath) const;
	virtual s2::SprPtr Create(const Json::Value& val, const std::string& dir) const;

}; // SpriteLoader

}

#endif // _GUM_SPRITE_LOADER_H_