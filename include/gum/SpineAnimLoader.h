#ifndef _GUM_SPINE_ANIM_LOADER_H_
#define _GUM_SPINE_ANIM_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/json.h>

#include <string>

namespace s2 { class AnimSymbol; }

namespace gum
{

class SpriteLoader;

class SpineAnimLoader : private cu::Uncopyable
{
public:
	SpineAnimLoader(s2::AnimSymbol* sym, const SpriteLoader* spr_loader = NULL);
	~SpineAnimLoader();

	void LoadJson(const Json::Value& val, const std::string& dir);

private:
	s2::AnimSymbol* m_sym;

	const SpriteLoader* m_spr_loader;

}; // SpineAnimLoader

}

#endif // _GUM_SPINE_ANIM_LOADER_H_