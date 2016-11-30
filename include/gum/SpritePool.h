#ifndef _GUM_SPRITE_POOL_H_
#define _GUM_SPRITE_POOL_H_

#include <CU_Singleton.h>

#include <map>

#include <stdint.h>

namespace s2 { class Sprite; }

namespace gum
{

//struct SpriteWrap
//{
//	uint32_t spr_id;
//	uint32_t node_id;
//	
//	uint8_t type;
//	uint8_t dirty;
//	uint8_t life;
//	uint8_t padding;
//	
//}; // SpriteWrap

class SpritePool
{
public:
	void GC();
	int Count() const { return m_sym_id_cache.size(); }

	s2::Sprite* FetchBySym(const uint32_t id);

private:
	//enum ResType
	//{
	//	RES_IMAGE = 0,
	//	RES_SCALE9,
	//	RES_ICON,
	//	RES_TEXTURE,
	//	RES_TEXTBOX,
	//	RES_COMPLEX,
	//	RES_ANIMATION,
	//	RES_ANIM2,
	//	RES_PARTICLE3D,
	//	RES_PARTICLE2D,
	//	RES_SHAPE,
	//	RES_MESH,
	//	RES_MASK,
	//	RES_TRAIL,
	//	RES_SKELETON,

	//	MAX_RES
	//};

private:
	struct Item
	{
		s2::Sprite* spr;
		int ref_count;
	};

private:
	std::map<uint32_t, s2::Sprite*> m_sym_id_cache;

	SINGLETON_DECLARATION(SpritePool);

}; // SpritePool

}

#endif // _GUM_SPRITE_POOL_H_