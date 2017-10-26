#ifndef _GUM_GTXT_H_
#define _GUM_GTXT_H_

#include <sprite2/pre_defined.h>
#include <sprite2/typedef.h>
#include <cu/cu_macro.h>
#include <cu/cu_stl.h>
#include <SM_Matrix.h>
#include S2_MAT_HEADER

struct gtxt_label_style;
struct gtxt_glyph_layout;

namespace s2 { class Color; class Symbol; }
namespace cooking { class DisplayList; }

namespace gum
{

class Color;

class GTxt
{
public:
	void Init(const CU_VEC<std::pair<CU_STR, CU_STR> >& fonts, 
		const CU_VEC<std::pair<CU_STR, CU_STR> >& user_fonts);

	void LoadFont(const CU_STR& name, const CU_STR& filepath);
	void LoadUserFont(const CU_STR& name, const CU_STR& filepath);
	void LoadUserFontChar(const CU_STR& str, const CU_STR& pkg, const CU_STR& node);

	void AddColor(const CU_STR& name, unsigned int color);

	void Draw(cooking::DisplayList* dlist, const gtxt_label_style& style, const S2_MAT& mt, 
		const s2::Color& mul, const s2::Color& add, const char* text, int time, bool richtext) const;
	void Draw(const S2_MAT& mt, const CU_STR& str, int width = 200) const;

	sm::vec2 GetSize(const gtxt_label_style& style, const CU_STR& text) const;

//	void Reload(const Sprite* spr);

	void Clear();

	void GetUFLayout(int unicode, int font, struct gtxt_glyph_layout* layout) const;
	void DrawUFChar(int unicode, int font, float x, float y, void* ud) const;

	static void SetCap(int cap_bitmap, int cap_layout);

private:
	static int m_cap_bitmap, m_cap_layout;

	CU_SET<CU_STR> m_fonts;

	CU_SET<CU_STR> m_colors;

	CU_MAP<int, s2::SymPtr> m_user_font_chars;
		
	CU_SINGLETON_DECLARATION(GTxt)

}; // GTxt
	
}

#endif // _GUM_GTXT_H_
