#ifndef _GUM_GLYPH_STYLE_H_
#define _GUM_GLYPH_STYLE_H_

#include <stdint.h>

struct gtxt_glyph_style;

namespace gum
{

class GlyphStyle
{
public:
	int      font;
	int      font_size;
	uint32_t font_color;

	int      edge;
	int      edge_size;
	uint32_t edge_color;

public:
	GlyphStyle();
	GlyphStyle(const gtxt_glyph_style* gs);

	bool operator == (const GlyphStyle& style) const;

}; // GlyphStyle

}

#endif // _GUM_GLYPH_STYLE_H_