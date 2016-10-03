#ifndef _GUM_SPINE_LOADER_H_
#define _GUM_SPINE_LOADER_H_

#include <CU_Uncopyable.h>

#include <string>

namespace s2 { class AnimSymbol; }

namespace gum
{

class SpriteLoader;

class SpineLoader : private cu::Uncopyable
{
public:
	SpineLoader(s2::AnimSymbol* sym, const SpriteLoader* spr_loader = NULL);
	~SpineLoader();

	void LoadJson(const std::string& filepath);

private:
	s2::AnimSymbol* m_sym;

	const SpriteLoader* m_spr_loader;

}; // SpineLoader

}

#endif // _GUM_SPINE_LOADER_H_