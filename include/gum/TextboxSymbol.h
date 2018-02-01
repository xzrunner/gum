#ifndef _GUM_TEXTBOX_SYMBOL_H_
#define _GUM_TEXTBOX_SYMBOL_H_

#include <sprite2/TextboxSymbol.h>
#include <cu/cu_stl.h>

namespace gum
{

class TextboxSymbol : public s2::TextboxSymbol
{
public:
	TextboxSymbol() {}
	TextboxSymbol(uint32_t id);

protected:
	/**
	 *  @interface
	 *    s2::TextboxSymbol
	 */
	virtual void DrawText(cooking::DisplayList* dlist, const gtxt_label_style& style, const S2_MAT& mt, 
		const pt2::Color& mul, const pt2::Color& add, const CU_STR& text, int time, bool richtext) const override;

}; // TextboxSymbol

}

#endif // _GUM_TEXTBOX_SYMBOL_H_
