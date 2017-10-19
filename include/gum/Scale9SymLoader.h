#ifndef _GUM_SCALE9_SYM_LOADER_H_
#define _GUM_SCALE9_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <json/json.h>

#include <memory>

#include <stdint.h>

namespace s2 { class Scale9Symbol; class Sprite; }
namespace simp { class NodeScale9; }

namespace gum
{

class SpriteLoader;

class Scale9SymLoader : private cu::Uncopyable
{
public:
	Scale9SymLoader(const std::shared_ptr<s2::Scale9Symbol>& sym, 
		const std::shared_ptr<const SpriteLoader>& spr_loader = NULL);

	void LoadJson(const CU_STR& filepath);
	void LoadBin(const simp::NodeScale9* node);

private:
	std::shared_ptr<s2::Sprite> LoadSprite(uint32_t sym_id, uint16_t dir, uint16_t mirror);

private:
	std::shared_ptr<s2::Scale9Symbol> m_sym;

	std::shared_ptr<const SpriteLoader> m_spr_loader;

}; // Scale9SymLoader

}

#endif // _GUM_SCALE9_SYM_LOADER_H_
