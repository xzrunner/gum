#include "gum/Painting2.h"
#include "gum/GTxt.h"

#include <gtxt_label.h>
#include <painting2/Callback.h>
#include <painting2/Color.h>
#include <painting2/Text.h>

#include <sm_const.h>

namespace
{

void copy_color(gtxt_glyph_color& dst, const pt2::GradientColor& src)
{
	if (src.items.size() == 3)
	{
		dst.mode_type = 2;
		dst.mode.THREE.begin_col.integer = src.items[0].col.ToRGBA();
		dst.mode.THREE.mid_col.integer = src.items[1].col.ToRGBA();
		dst.mode.THREE.end_col.integer = src.items[2].col.ToRGBA();
		dst.mode.THREE.begin_pos = src.items[0].pos;
		dst.mode.THREE.mid_pos = src.items[1].pos;
		dst.mode.THREE.end_pos = src.items[2].pos;
		dst.mode.THREE.angle = src.angle;
	}
	else if (src.items.size() == 2)
	{
		dst.mode_type = 1;
		dst.mode.TWO.begin_col.integer = src.items[0].col.ToRGBA();
		dst.mode.TWO.end_col.integer = src.items[1].col.ToRGBA();
		dst.mode.TWO.begin_pos = src.items[0].pos;
		dst.mode.TWO.end_pos = src.items[1].pos;
		dst.mode.TWO.angle = src.angle;
	}
	else
	{
		assert(src.items.size() == 1);
		dst.mode_type = 0;
		dst.mode.ONE.color.integer = src.items[0].col.ToRGBA();
	}
}

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
	copy_color(ls.gs.font_color, tb.font_color);

	ls.gs.edge      = tb.has_edge;
	ls.gs.edge_size = tb.edge_size;
	copy_color(ls.gs.edge_color, tb.edge_color);

	gum::GTxt::Instance()->Draw(nullptr, ls, mat, 
		pt2::Color(255, 255, 255), pt2::Color(0, 0, 0, 0), text.text.c_str(), 0, false);
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