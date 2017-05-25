#ifndef _GUM_ANIM_SYM_LOADER_H_
#define _GUM_ANIM_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <string>

namespace s2 { class AnimSymbol; }
namespace simp { class NodeAnimation; }

namespace gum
{

class SymbolLoader;
class SpriteLoader;

class AnimSymLoader : private cu::Uncopyable
{
public:
	AnimSymLoader(s2::AnimSymbol* sym, const SymbolLoader* sym_loader = NULL,
		const SpriteLoader* spr_loader = NULL, bool flatten = false);
	~AnimSymLoader();

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeAnimation* node);

private:
	s2::AnimSymbol* m_sym;

	const SymbolLoader* m_sym_loader;
	const SpriteLoader* m_spr_loader;

	bool m_flatten;

}; // AnimSymLoader

}

#endif // _GUM_ANIM_SYM_LOADER_H_
