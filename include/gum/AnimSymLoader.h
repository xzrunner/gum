#ifndef _GUM_ANIM_SYM_LOADER_H_
#define _GUM_ANIM_SYM_LOADER_H_

#include <cu/uncopyable.h>

#include <string>
#include <memory>

namespace s2 { class AnimSymbol; }
namespace simp { class NodeAnimation; }

namespace gum
{

class SymbolLoader;
class SpriteLoader;

class AnimSymLoader : private cu::Uncopyable
{
public:
	AnimSymLoader(const std::shared_ptr<s2::AnimSymbol>& sym, 
		const std::shared_ptr<const SymbolLoader>& sym_loader = NULL,
		const std::shared_ptr<const SpriteLoader>& spr_loader = NULL);

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeAnimation* node);

private:
	std::shared_ptr<s2::AnimSymbol> m_sym;

	std::shared_ptr<const SymbolLoader> m_sym_loader;
	std::shared_ptr<const SpriteLoader> m_spr_loader;

}; // AnimSymLoader

}

#endif // _GUM_ANIM_SYM_LOADER_H_
