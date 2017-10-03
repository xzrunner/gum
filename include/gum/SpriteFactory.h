#ifndef _GUM_SPRITE_FACTORY_H_
#define _GUM_SPRITE_FACTORY_H_

#include <cu/cu_macro.h>
#include <sprite2/s2_typedef.h>

#include <json/json.h>

#include <string>

#include <stdint.h>

namespace gum
{

class SpriteFactory
{
public:
	s2::SprPtr Create(const s2::SymPtr& sym, uint32_t id = -1, bool create_actors = true) const;

	/**
	 *  @brief
	 *    raw, json res
	 */	
	s2::SprPtr Create(const std::string& filepath) const;
	s2::SprPtr Create(const Json::Value& val, const std::string& dir) const;

	/**
	 *  @brief
	 *    pack, binary res
	 */	
	s2::SprPtr Create(uint32_t id);
	s2::SprPtr CreateFromSym(uint32_t id, bool create_actors = true);

private:
	static void CreateSprActors(const s2::SprPtr& spr);

	CU_SINGLETON_DECLARATION(SpriteFactory);

}; // SpriteFactory

}

#endif // _GUM_SPRITE_FACTORY_H_
