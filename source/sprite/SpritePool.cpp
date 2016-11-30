#include "SpritePool.h"
#include "SpriteFactory.h"

namespace gum
{

SINGLETON_DEFINITION(SpritePool);

SpritePool::SpritePool()
{
}

void SpritePool::GC()
{
	
}

s2::Sprite* SpritePool::FetchBySym(const uint32_t id)
{
	std::map<uint32_t, s2::Sprite*>::iterator itr 
		= m_sym_id_cache.find(id);
	if (itr != m_sym_id_cache.end()) {
		
	}
}

}