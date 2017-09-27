#ifndef _GUM_SCALE9_SYM_LOADER_H_
#define _GUM_SCALE9_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/json.h>

#include <string>

#include <stdint.h>

namespace s2 { class Scale9Symbol; class Sprite; }
namespace simp { class NodeScale9; }

namespace gum
{

class SpriteLoader;

class Scale9SymLoader : private cu::Uncopyable
{
public:
	Scale9SymLoader(s2::Scale9Symbol* sym, const SpriteLoader* spr_loader = NULL);
	~Scale9SymLoader();

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeScale9* node);

private:
	s2::Sprite* LoadSprite(uint32_t sym_id, uint16_t dir, uint16_t mirror);

private:
	s2::Scale9Symbol* m_sym;

	const SpriteLoader* m_spr_loader;

}; // Scale9SymLoader

}

#endif // _GUM_SCALE9_SYM_LOADER_H_
