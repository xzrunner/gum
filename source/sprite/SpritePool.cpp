#include "gum/SpritePool.h"
#include "gum/SpriteFactory.h"

#include <sprite2/Sprite.h>
#include <sprite2/Symbol.h>

namespace gum
{

CU_SINGLETON_DEFINITION(SpritePool);

SpritePool::SpritePool()
{
}

void SpritePool::GC()
{
	auto itr = m_sym_id_cache.begin();
	for (; itr != m_sym_id_cache.end(); ) {
		if (itr->second.expired()) {
			itr = m_sym_id_cache.erase(itr);
		} else {
			++itr;
		}
	}
}

s2::SprPtr SpritePool::Fetch(const uint32_t sym_id)
{
	auto itr = m_sym_id_cache.find(sym_id);
	if (itr != m_sym_id_cache.end()) {
		return itr->second.lock();
	} else {
		return SpriteFactory::Instance()->CreateFromSym(sym_id, true);
	}
}

void SpritePool::Return(const s2::SprPtr& spr)
{
	int sym_id = spr->GetSymbol()->GetID();
	auto itr = m_sym_id_cache.find(sym_id);
	if (itr == m_sym_id_cache.end()) {
		m_sym_id_cache.insert(std::make_pair(sym_id, spr));
	}
}

}