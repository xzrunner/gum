#include "ResourceUID.h"
#include "GlyphStyle.h"

namespace gum
{

ResourceUID::Type ResourceUID::GetType(UID uid)
{
	return static_cast<ResourceUID::Type>((uid & TYPE_MASK) >> DATA_SIZE);
}

UID ResourceUID::RawFile(const std::string& filepath)
{
	return Compose(HashString(filepath), RES_RAW_FILE);
}

UID ResourceUID::Glyph(int unicode, const GlyphStyle& gs)
{
	uint64_t hash_style = 0;
	uint64_t seed = 31;

	hash_style = hash_style * seed + gs.font;
	hash_style = hash_style * seed + gs.font_size;
	hash_style = hash_style * seed + (gs.font_color >> 24) & 0xff;
	hash_style = hash_style * seed + (gs.font_color >> 16) & 0xff;
	hash_style = hash_style * seed + (gs.font_color >> 8) & 0xff;
	hash_style = hash_style * seed + (gs.font_color) & 0xff;	
	if (gs.edge) 
	{
		hash_style = hash_style * seed + gs.edge;
		hash_style = hash_style * seed + gs.edge_size;
		hash_style = hash_style * seed + (gs.edge_color >> 24) & 0xff;
		hash_style = hash_style * seed + (gs.edge_color >> 16) & 0xff;
		hash_style = hash_style * seed + (gs.edge_color >> 8) & 0xff;
		hash_style = hash_style * seed + (gs.edge_color) & 0xff;
	}
	uint64_t hash = ((hash_style & 0xffffffff) << 32) | unicode;

	return Compose(hash, RES_GLYPH);
}

UID ResourceUID::BinNode(uint32_t node_id)
{
	return Compose(node_id, RES_BIN_NODE);
}

UID ResourceUID::Sprite(int spr_id)
{
	return Compose(spr_id, RES_SPRITE);
}

UID ResourceUID::Actor(const s2::Actor* actor)
{
	uint64_t actor_id = reinterpret_cast<uint64_t>(actor);
	return Compose(actor_id, RES_ACTOR);
}

uint64_t ResourceUID::HashString(const std::string& str)
{
	const char* c = str.c_str();

	// BKDR Hash Function
	uint64_t seed = 131; // 31 131 1313 13131 131313 etc..
	uint64_t hash = 0;

	while (*c) {
		hash = hash * seed + (*c++);
	}

	return hash;
}

UID ResourceUID::Compose(uint64_t data, Type type)
{
	return (data & DATA_MASK) | ((uint64_t)type << DATA_SIZE);
}

}