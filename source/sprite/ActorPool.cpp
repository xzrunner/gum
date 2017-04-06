#include "ActorPool.h"
#include "SpriteFactory.h"

#include <sprite2/S2_Sprite.h>
#include <sprite2/S2_Actor.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/ActorFactory.h>

namespace gum
{

SINGLETON_DEFINITION(ActorPool);

ActorPool::ActorPool()
{
}

void ActorPool::GC()
{	
	std::map<uint32_t, s2::Actor*>::iterator itr = m_sym_id_cache.begin();
	for ( ; itr != m_sym_id_cache.end(); ++itr) {
		s2::Actor* actor = itr->second;
		actor->GetSpr()->RemoveReference();
	}
	m_sym_id_cache.clear();
}

s2::Actor* ActorPool::Fetch(const uint32_t sym_id, bool& is_new)
{
	std::map<uint32_t, s2::Actor*>::iterator itr = m_sym_id_cache.find(sym_id);
	if (itr != m_sym_id_cache.end()) {
		is_new = false;
		s2::Actor* actor = itr->second;
		actor->GetSpr()->AddReference();
		return actor;
	} else {
		is_new = true;
		s2::Sprite* spr = SpriteFactory::Instance()->CreateFromSym(sym_id, true);
		return s2::ActorFactory::Instance()->Create(NULL, spr);
	}
}

void ActorPool::Return(s2::Actor* actor)
{
	int sym_id = actor->GetSpr()->GetSymbol()->GetID();
	std::map<uint32_t, s2::Actor*>::iterator itr 
		= m_sym_id_cache.find(sym_id);
	if (itr == m_sym_id_cache.end()) {
		actor->GetSpr()->AddReference();
		m_sym_id_cache.insert(std::make_pair(sym_id, actor));
	}
}

}