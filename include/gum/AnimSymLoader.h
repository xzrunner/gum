#ifndef _GUM_ANIM_SYM_LOADER_H_
#define _GUM_ANIM_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <string>

namespace s2 { class AnimSymbol; }
namespace simp { class NodeAnimation; }

namespace gum
{

class SpriteLoader;

class AnimSymLoader : private cu::Uncopyable
{
public:
	AnimSymLoader(s2::AnimSymbol* sym, const SpriteLoader* spr_loader = NULL);
	~AnimSymLoader();

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeAnimation* node);

private:
	s2::AnimSymbol* m_sym;

	const SpriteLoader* m_spr_loader;

}; // AnimSymLoader

}

#endif // _GUM_ANIM_SYM_LOADER_H_
