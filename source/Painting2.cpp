#include "gum/Painting2.h"
#include "gum/GTxt.h"

#include <gtxt_label.h>
#include <painting2/Callback.h>
#include <painting2/Color.h>
#include <painting2/Text.h>

namespace
{

void draw_text(const pt2::Text& text, const sm::Matrix2D& mat)
{
	auto& tb = text.tb;

	gtxt_label_style ls;

	ls.width   = tb.width;
	ls.height  = tb.height;
	ls.align_h = tb.align_hori;
	ls.align_v = tb.align_vert;
	ls.space_h = tb.space_hori;
	ls.space_v = tb.space_vert;
	ls.overflow = tb.overflow;

	ls.gs.font = tb.font_type;
	ls.gs.font_size = tb.font_size;
	ls.gs.font_color.integer = tb.font_color.ToRGBA();

	ls.gs.edge               = tb.has_edge;
	ls.gs.edge_size          = tb.edge_size;
	ls.gs.edge_color.integer = tb.edge_color.ToRGBA();

	gum::GTxt::Instance()->Draw(nullptr, ls, mat, 
		pt2::Color(1, 1, 1), pt2::Color(0, 0, 0), text.text.c_str(), 0, false);
}

}

namespace gum
{

CU_SINGLETON_DEFINITION(Painting2);

Painting2::Painting2()
{
}

void Painting2::Init()
{
	pt2::Callback::Funs cb;
	cb.draw_text = draw_text;

	pt2::Callback::RegisterCallback(cb);
}

}