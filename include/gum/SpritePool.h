#ifndef _GUM_SPRITE_POOL_H_
#define _GUM_SPRITE_POOL_H_

#include <CU_Singleton.h>

#include <map>

#include <stdint.h>

namespace s2 { class Sprite; }

namespace gum
{

class SpritePool
{
public:
	void GC();
	int Count() const { return m_sym_id_cache.size(); }

	s2::Sprite* Fetch(const uint32_t sym_id);
	void Return(s2::Sprite* spr);

private:
	std::map<uint32_t, s2::Sprite*> m_sym_id_cache;

	SINGLETON_DECLARATION(SpritePool);

}; // SpritePool

}

#endif // _GUM_SPRITE_POOL_H_