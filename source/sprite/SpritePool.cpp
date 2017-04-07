#include "SpritePool.h"
#include "SpriteFactory.h"

#include <sprite2/S2_Sprite.h>
#include <sprite2/S2_Symbol.h>

namespace gum
{

SINGLETON_DEFINITION(SpritePool);

SpritePool::SpritePool()
{
}

void SpritePool::GC()
{	
	while (true)
	{
		bool dirty = false;

		std::map<uint32_t, s2::Sprite*>::iterator itr = m_sym_id_cache.begin();
		while (itr != m_sym_id_cache.end())
		{
			if (itr->second->GetRefCount() == 1) {
				itr->second->RemoveReference();
				m_sym_id_cache.erase(itr++);
				dirty = true;
			} else {
				++itr;
			}
		}

		if (!dirty) {
			break;
		}
	}
}

s2::Sprite* SpritePool::Fetch(const uint32_t sym_id)
{
	std::map<uint32_t, s2::Sprite*>::iterator itr = m_sym_id_cache.find(sym_id);
	if (itr != m_sym_id_cache.end()) {
		s2::Sprite* ret = itr->second;
		ret->AddReference();
		return ret;
	} else {
		return SpriteFactory::Instance()->CreateFromSym(sym_id, true);
	}
}

void SpritePool::Return(s2::Sprite* spr)
{
	int sym_id = spr->GetSymbol()->GetID();
	std::map<uint32_t, s2::Sprite*>::iterator itr 
		= m_sym_id_cache.find(sym_id);
	if (itr == m_sym_id_cache.end()) {
		spr->AddReference();
		m_sym_id_cache.insert(std::make_pair(sym_id, spr));
	}
}

}