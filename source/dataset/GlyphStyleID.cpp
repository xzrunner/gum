#include "GlyphStyleID.h"

namespace gum
{

SINGLETON_DEFINITION(GlyphStyleID);

GlyphStyleID::GlyphStyleID()
	: m_next_id(0)
{
}

int GlyphStyleID::Gen(const GlyphStyle& style)
{
 	if (style == m_last.first) {
 		return m_last.second;
 	}

	int hash = Hash(style) % HASH_CAP;
	std::vector<std::pair<GlyphStyle, int> >& list = m_hash[hash];
	for (int i = 0, n = list.size(); i < n; ++i) {
		if (list[i].first == style) {
			m_last = list[i];
			return list[i].second;
		}
	}

	int id = m_next_id++;
	list.push_back(std::make_pair(style, id));
	return id;
}

int GlyphStyleID::Hash(const GlyphStyle& gs)
{
	int hash = 0;
	int seed = 31;

	hash = hash * seed + gs.font;
	hash = hash * seed + gs.font_size;
	hash = hash * seed + (gs.font_color >> 24) & 0xff;
	hash = hash * seed + (gs.font_color >> 16) & 0xff;
	hash = hash * seed + (gs.font_color >> 8) & 0xff;
	hash = hash * seed + (gs.font_color) & 0xff;	
	if (gs.edge) 
	{
		hash = hash * seed + gs.edge;
		hash = hash * seed + gs.edge_size;
		hash = hash * seed + (gs.edge_color >> 24) & 0xff;
		hash = hash * seed + (gs.edge_color >> 16) & 0xff;
		hash = hash * seed + (gs.edge_color >> 8) & 0xff;
		hash = hash * seed + (gs.edge_color) & 0xff;
	}
	return hash;
}

}