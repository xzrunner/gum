#include "GUM_TextboxSymbol.h"
#include "GUM_GTxt.h"
#include "gum/StringHelper.h"

namespace gum
{

TextboxSymbol::TextboxSymbol(uint32_t id)
	: s2::TextboxSymbol(id)
{
}

void TextboxSymbol::DrawText(const gtxt_label_style& style, const S2_MAT& mt, const s2::Color& mul, const s2::Color& add, const std::string& text, int time, bool richtext) const
{
	// todo defer
	GTxt::Instance()->Draw(style, mt, mul, add, text, time, richtext);
}

}