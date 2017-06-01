#ifndef _GUM_SPRITE_LOADER_H_
#define _GUM_SPRITE_LOADER_H_

#include <CU_RefCountObj.h>

#include <json/json.h>

namespace s2 { class Sprite; class Symbol; }

namespace gum
{

class SpriteLoader : public cu::RefCountObj
{
public:
	virtual s2::Sprite* Create(s2::Symbol* sym) const;	
	virtual s2::Sprite* Create(const std::string& filepath) const;
	virtual s2::Sprite* Create(const Json::Value& val, const std::string& dir, bool flatten = false) const;

}; // SpriteLoader

}

#endif // _GUM_SPRITE_LOADER_H_