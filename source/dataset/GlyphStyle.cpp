#include "GlyphStyle.h"

#include <gtxt_glyph.h>

namespace gum
{

GlyphStyle::GlyphStyle(const gtxt_glyph_style* gs)
	: font(gs->font)
	, font_size(gs->font_size)
	, font_color(gs->font_color.integer)
	, edge(gs->edge)
	, edge_size(static_cast<int>(gs->edge_size))
	, edge_color(gs->edge_color.integer)
{}

}