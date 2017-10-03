#ifndef _GUM_GTXT_H_
#define _GUM_GTXT_H_

#include <sprite2/pre_defined.h>
#include <sprite2/s2_typedef.h>
#include <cu/cu_macro.h>
#include <SM_Matrix.h>
#include S2_MAT_HEADER

#include <string>
#include <vector>
#include <map>
#include <set>

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
	void Init(const std::vector<std::pair<std::string, std::string> >& fonts, 
		const std::vector<std::pair<std::string, std::string> >& user_fonts);

	void LoadFont(const std::string& name, const std::string& filepath);
	void LoadUserFont(const std::string& name, const std::string& filepath);
	void LoadUserFontChar(const std::string& str, const std::string& pkg, const std::string& node);

	void AddColor(const std::string& name, unsigned int color);

	void Draw(cooking::DisplayList* dlist, const gtxt_label_style& style, const S2_MAT& mt, 
		const s2::Color& mul, const s2::Color& add, const std::string& text, int time, bool richtext) const;
	void Draw(const S2_MAT& mt, const std::string& str, int width = 200) const;

	sm::vec2 GetSize(const gtxt_label_style& style, const std::string& text) const;

//	void Reload(const Sprite* spr);

	void Clear();

	void GetUFLayout(int unicode, int font, struct gtxt_glyph_layout* layout) const;
	void DrawUFChar(int unicode, int font, float x, float y, void* ud) const;

	static void SetCap(int cap_bitmap, int cap_layout);

private:
	static int m_cap_bitmap, m_cap_layout;

	std::set<std::string> m_fonts;

	std::set<std::string> m_colors;

	std::map<int, s2::SymPtr> m_user_font_chars;
		
	CU_SINGLETON_DECLARATION(GTxt)

}; // GTxt
	
}

#endif // _GUM_GTXT_H_
