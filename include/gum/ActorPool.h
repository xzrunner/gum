#ifndef _GUM_ACTOR_POOL_H_
#define _GUM_ACTOR_POOL_H_

#include <CU_Singleton.h>

#include <map>

#include <stdint.h>

namespace s2 { class Actor; }

namespace gum
{

class ActorPool
{
public:
	void GC();
	int Count() const { return m_sym_id_cache.size(); }

	s2::Actor* Fetch(const uint32_t sym_id, bool& is_new);
	void Return(s2::Actor* actor);

private:
	std::map<uint32_t, s2::Actor*> m_sym_id_cache;

	SINGLETON_DECLARATION(ActorPool);

}; // ActorPool

}

#endif // _GUM_ACTOR_POOL_H_