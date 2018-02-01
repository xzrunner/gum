#include "gum/TextboxSymbol.h"
#include "gum/GTxt.h"

namespace gum
{

TextboxSymbol::TextboxSymbol(uint32_t id)
	: s2::TextboxSymbol(id)
{
}

void TextboxSymbol::DrawText(cooking::DisplayList* dlist, const gtxt_label_style& style, const S2_MAT& mt, const pt2::Color& mul, const pt2::Color& add, const CU_STR& text, int time, bool richtext) const
{
	gum::GTxt::Instance()->Draw(dlist, style, mt, mul, add, text.c_str(), time, richtext);
}

}