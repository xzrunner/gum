#ifndef _GUM_GLYPH_STYLE_ID_H_
#define _GUM_GLYPH_STYLE_ID_H_

#include "GlyphStyle.h"

#include <CU_Singleton.h>

#include <vector>

namespace gum
{

class GlyphStyle;

class GlyphStyleID
{
public:
	int Gen(const GlyphStyle& style);

private:
	static int Hash(const GlyphStyle& style);

private:
	static const int HASH_CAP = 197;

private:
	int m_next_id;
	std::vector<std::pair<GlyphStyle, int> > m_hash[HASH_CAP];

 	std::pair<GlyphStyle, int> m_last;
	
	SINGLETON_DECLARATION(GlyphStyleID)

}; // GlyphStyleID

}

#endif // _GUM_GLYPH_STYLE_ID_H_