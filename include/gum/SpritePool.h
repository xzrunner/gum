#ifndef _GUM_SPRITE_POOL_H_
#define _GUM_SPRITE_POOL_H_

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>
#include <sprite2/typedef.h>

#include <stdint.h>

namespace s2 { class Sprite; }

namespace gum
{

class SpritePool
{
public:
	void GC();
	int Count() const { return m_sym_id_cache.size(); }

	s2::SprPtr Fetch(const uint32_t sym_id);
	void Return(const s2::SprPtr& spr);

private:
	CU_MAP<uint32_t, std::weak_ptr<s2::Sprite>> m_sym_id_cache;

	CU_SINGLETON_DECLARATION(SpritePool);

}; // SpritePool

}

#endif // _GUM_SPRITE_POOL_H_