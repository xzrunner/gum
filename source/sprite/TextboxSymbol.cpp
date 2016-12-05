#include "TextboxSymbol.h"
#include "GUM_GTxt.h"
#include "StringHelper.h"

namespace gum
{

TextboxSymbol::TextboxSymbol(uint32_t id)
	: s2::TextboxSymbol(id)
{
}

void TextboxSymbol::DrawText(const gtxt_label_style& style, const sm::mat4& mt, const s2::Color& mul, const s2::Color& add, const std::string& text, int time, bool richtext) const
{
	GTxt::Instance()->Draw(style, mt, mul, add, text, time, richtext);
}

}