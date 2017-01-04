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
	uint64_t hash_style;
	if (gs.edge) {
		hash_style = 
			(gs.font      * 97)  ^ 
			(gs.font_size * 101) ^
			(gs.font_color)      ^ 
			(gs.edge      * 233) ^
			(gs.edge_size * 787) ^
			(gs.edge_color);
	} else {
		hash_style = 
			(gs.font      * 97)  ^ 
			(gs.font_size * 101) ^
			(gs.font_color);
	}

	uint64_t hash = ((hash_style & 0xffffffff) << 32) | unicode;

	return Compose(hash, RES_GLYPH);
}

UID ResourceUID::BinNode(uint32_t node_id)
{
	return Compose(node_id, RES_BIN_NODE);
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