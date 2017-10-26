#include "gum/ActorPool.h"
#include "gum/SpriteFactory.h"

#include <sprite2/Sprite.h>
#include <sprite2/Actor.h>
#include <sprite2/Symbol.h>
#include <sprite2/ActorFactory.h>

namespace gum
{

CU_SINGLETON_DEFINITION(ActorPool);

ActorPool::ActorPool()
{
}

void ActorPool::GC()
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

s2::ActorPtr ActorPool::Fetch(const uint32_t sym_id, bool& is_new)
{
	auto itr = m_sym_id_cache.find(sym_id);
	if (itr != m_sym_id_cache.end()) {
		is_new = false;
		return itr->second.lock();
	} else {
		is_new = true;
		auto spr = SpriteFactory::Instance()->CreateFromSym(sym_id, true);
		if (spr) {
			return s2::ActorFactory::Create(NULL, spr);
		} else {
			return NULL;
		}
	}
}

void ActorPool::Return(const s2::ActorPtr& actor)
{
	int sym_id = actor->GetSprRaw()->GetSymbol()->GetID();
	auto itr = m_sym_id_cache.find(sym_id);
	if (itr == m_sym_id_cache.end()) {
		m_sym_id_cache.insert(std::make_pair(sym_id, actor));
	}
}

}