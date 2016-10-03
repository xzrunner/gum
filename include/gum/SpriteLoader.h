#ifndef _GUM_SPRITE_LOADER_H_
#define _GUM_SPRITE_LOADER_H_

#include <CU_RefCountObj.h>

#include <json/json.h>

namespace s2 { class Sprite; }

namespace gum
{

class SpriteLoader : public cu::RefCountObj
{
public:
	virtual s2::Sprite* Load(const Json::Value& val, const std::string& dir) const;
	virtual s2::Sprite* Load(const std::string& filepath) const;

}; // SpriteLoader

}

#endif // _GUM_SPRITE_LOADER_H_