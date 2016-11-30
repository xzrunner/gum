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
	return NULL;
}

}