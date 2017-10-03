#ifndef _GUM_ACTOR_POOL_H_
#define _GUM_ACTOR_POOL_H_

#include <cu/cu_macro.h>
#include <sprite2/s2_typedef.h>

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

	s2::ActorPtr Fetch(const uint32_t sym_id, bool& is_new);
	void Return(const s2::ActorPtr& actor);

private:
	std::map<uint32_t, std::weak_ptr<s2::Actor>> m_sym_id_cache;

	CU_SINGLETON_DECLARATION(ActorPool);

}; // ActorPool

}

#endif // _GUM_ACTOR_POOL_H_