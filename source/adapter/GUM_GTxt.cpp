#include "GUM_GTxt.h"
#include "gum/StringHelper.h"
#include "SymbolPool.h"
#include "GUM_ImageSymbol.h"
#include "Image.h"
#include "GlyphStyle.h"
#include "GUM_DTex.h"
#include "ResourceUID.h"

#include <gtxt_util.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/Shape2Shader.h>
#include <simp/NodeFactory.h>
#include <sprite2/S2_RVG.h>
#include <sprite2/RenderColor.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/RenderParams.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/DrawNode.h>

#include <json/json.h>

extern "C" {
	#include <gtxt.h>
}

#include <fstream>

namespace gum
{

SINGLETON_DEFINITION(GTxt)

int GTxt::m_cap_bitmap = 50;
int GTxt::m_cap_layout = 500;

/************************************************************************/
/* render                                                               */
/************************************************************************/

struct render_params
{
	const S2_MAT* mt;
	const s2::Color* mul;
	const s2::Color* add;
};

static void
render_glyph(int id, const float* _texcoords, float x, float y, float w, float h, const gtxt_draw_style* ds, render_params* rp) 
{
	x += ds->offset_x;
	y += ds->offset_y;
	float hw = w * 0.5f * ds->scale, hh = h * 0.5f * ds->scale;

	sm::vec2 vertices[4];
	vertices[0] = sm::vec2(x - hw, y - hh);
	vertices[1] = sm::vec2(x + hw, y - hh);
	vertices[2] = sm::vec2(x + hw, y + hh);
	vertices[3] = sm::vec2(x - hw, y + hh);
	for (int i = 0; i < 4; ++i) {
		vertices[i] = *rp->mt * vertices[i];
	}

	sm::vec2 texcoords[4];
	texcoords[0].Set(_texcoords[0], _texcoords[1]);
	texcoords[1].Set(_texcoords[2], _texcoords[3]);
	texcoords[2].Set(_texcoords[4], _texcoords[5]);
	texcoords[3].Set(_texcoords[6], _texcoords[7]);

	s2::RenderColor color;
	if (rp->mul) {
		s2::Color multi_col = *rp->mul;
		multi_col.a = static_cast<int>(multi_col.a * ds->alpha);
		color.SetMul(multi_col);
	} 
	if (rp->add) {
		color.SetAdd(*rp->add);
	}

	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	if (sl_mgr->GetShaderType() == sl::FILTER) {
		sl::FilterShader* filter = static_cast<sl::FilterShader*>(sl_mgr->GetShader());
		if (filter->GetMode() != sl::FM_GRAY) {
			filter->SetColor(color.GetMulABGR(), color.GetAddABGR());
		}
		filter->Draw(&vertices[0].x, &texcoords[0].x, id);
	} else {
		sl_mgr->SetShader(sl::SPRITE2);
	 	sl::Sprite2Shader* sl_shader = static_cast<sl::Sprite2Shader*>(sl_mgr->GetShader());
	 	sl_shader->SetColor(color.GetMulABGR(), color.GetAddABGR());
	 	sl_shader->SetColorMap(color.GetRMapABGR(), color.GetGMapABGR(), color.GetBMapABGR());
	 	sl_shader->DrawQuad(&vertices[0].x, &texcoords[0].x, id);
	}
}

static void 
render_decoration(const S2_MAT& mat, float x, float y, float w, float h, const gtxt_draw_style* ds)
{
	const gtxt_decoration* d = &ds->decoration;
	if (d->type == GRDT_NULL) {
		return;
	}

	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	sl_mgr->SetShader(sl::SHAPE2);
	sl::Shape2Shader* sl_shader = static_cast<sl::Shape2Shader*>(sl_mgr->GetShader());
	sl_shader->SetColor(d->color);

	float hw = w * 0.5f,
		hh = h * 0.5f;
	if (d->type == GRDT_OVERLINE || d->type == GRDT_UNDERLINE || d->type == GRDT_STRIKETHROUGH) {
		sm::vec2 left(x - hw, y), right(x + hw, y);
		switch (d->type) 
		{
		case GRDT_OVERLINE:
			left.y = right.y = ds->row_y + ds->row_h;
			break;
		case GRDT_UNDERLINE:
			left.y = right.y = ds->row_y;
			break;
		case GRDT_STRIKETHROUGH:
			left.y = right.y = ds->row_y + ds->row_h * 0.5f;
			break;
		}
		s2::RVG::Line(mat * left, mat * right);
	} else if (d->type == GRDT_BORDER || d->type == GRDT_BG) {
		sm::vec2 min(x - hw, ds->row_y), 
			max(x + hw, ds->row_y + ds->row_h);
		min = mat * min;
		max = mat * max;
		if (d->type == GRDT_BG) {
			s2::RVG::Rect(min, max, true);
		} else if (ds->pos_type != GRPT_NULL) {
			s2::RVG::Line(min, sm::vec2(max.x, min.y));
			s2::RVG::Line(sm::vec2(min.x, max.y), max);
			if (ds->pos_type == GRPT_BEGIN) {
				s2::RVG::Line(min, sm::vec2(min.x, max.y));
			}
			if (ds->pos_type == GRPT_END) {
				s2::RVG::Line(sm::vec2(max.x, min.y), max);
			}
		}
	}
}

static void 
render(int id, const float* texcoords, float x, float y, float w, float h, const gtxt_draw_style* ds, void* ud) 
{
	render_params* rp = (render_params*)ud;
	if (ds) {
		if (ds->decoration.type == GRDT_BG) {
			render_decoration(*rp->mt, x, y, w, h, ds);
			render_glyph(id, texcoords, x, y, w, h, ds, rp);
		} else {
			render_glyph(id, texcoords, x, y, w, h, ds, rp);
			render_decoration(*rp->mt, x, y, w, h, ds);
		}
	} else {
		struct gtxt_draw_style ds;
		ds.alpha = 1;
		ds.scale = 1;
		ds.offset_x = ds.offset_y = 0;
		render_glyph(id, texcoords, x, y, w, h, &ds, rp);
	}
}

static void
draw_glyph(int unicode, float x, float y, float w, float h, 
		   const gtxt_glyph_style* gs, const gtxt_draw_style* ds, void* ud) 
{	
	int tex_id;
	int ft_count = gtxt_ft_get_font_cout();
	UID uid = ResourceUID::Glyph(unicode, GlyphStyle(gs));
	const float* texcoords = DTex::Instance()->QuerySymbol(uid, &tex_id);

	if( texcoords && (gs->font<ft_count) && !DTex::Instance()->ExistGlyph(uid) ) {
		texcoords = NULL;
	}

	if (texcoords) {
		render(tex_id, texcoords, x, y, w, h, ds, ud);
	} else {
		if (gs->font < ft_count) {
			float texcoords[8];
			if (DTex::Instance()->QueryGlyph(uid, texcoords, tex_id)) {
				render(tex_id, texcoords, x, y, w, h, ds, ud);
			} else {
				struct gtxt_glyph_layout layout;
				uint32_t* bmp = gtxt_glyph_get_bitmap(unicode, gs, &layout);
				if (!bmp) {
					return;
				}
				w = layout.sizer.width;
				h = layout.sizer.height;
				DTex::Instance()->LoadGlyph(bmp, w, h, uid);
			}
		} else {
			int uf_font = gs->font - ft_count;
			GTxt::Instance()->DrawUFChar(unicode, uf_font, x, y, ud);
		}
	}
}

/************************************************************************/
/* richtext extern symbol                                               */
/************************************************************************/

static void*
ext_sym_create(const char* str) {
	std::string src = StringHelper::UTF8ToGBK(str);
	std::string::size_type pos = src.find("export");
	if (pos != std::string::npos) {
		src.insert(pos, " ");
	}
	std::vector<std::string> tokens;
	StringHelper::Split(src, " =", tokens);
	s2::Symbol* sym = NULL;
	if (tokens.size() == 2) {
		if (tokens[0] == "path") {
			sym = SymbolPool::Instance()->Fetch(tokens[1]);
		}
	} else if (tokens.size() == 4) {
		if (tokens[0] == "pkg" && tokens[2] == "export") {
			uint32_t id = simp::NodeFactory::Instance()->GetNodeID(tokens[1], tokens[3]);
			if (id != 0xffffffff) {
				// todo: flatten
				sym = SymbolPool::Instance()->Fetch(id, false);
			}
		}
	}
	return sym;
}

static void
ext_sym_release(void* ext_sym) {
	if (ext_sym) {
		s2::Symbol* sym = static_cast<s2::Symbol*>(ext_sym);
		sym->RemoveReference();
	}
}

static void 
ext_sym_get_size(void* ext_sym, int* width, int* height) {
	if (!ext_sym) {
		*width= *height = 0;
		return;
	}

	s2::Symbol* sym = static_cast<s2::Symbol*>(ext_sym);
	sm::vec2 sz = sym->GetBounding().Size();
	*width  = static_cast<int>(sz.x);
	*height = static_cast<int>(sz.y);
}

static void
ext_sym_render(void* ext_sym, float x, float y, void* ud) {
	if (!ext_sym) {
		return;
	}

	s2::RenderParams rp;
	render_params* _rp = (render_params*)ud;
	if (_rp->mt) {
		rp.mt = *_rp->mt;
	}
	if (_rp->mul) {
		rp.color.SetMul(*_rp->mul);
	}
	if (_rp->add) {
		rp.color.SetAdd(*_rp->add);
	}
	s2::DrawNode::Draw(static_cast<s2::Symbol*>(ext_sym), rp, sm::vec2(x, y));
}

/************************************************************************/
/* user font                                                            */
/************************************************************************/

static void 
get_uf_layout(int unicode, int font, struct gtxt_glyph_layout* layout) {
	GTxt::Instance()->GetUFLayout(unicode, font, layout);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

GTxt::GTxt() 
{
	gtxt_label_cb_init(draw_glyph);

	gtxt_ft_create();

	gtxt_glyph_create(m_cap_bitmap, m_cap_layout, NULL, get_uf_layout);

	gtxt_richtext_ext_sym_cb_init(&ext_sym_create, &ext_sym_release, &ext_sym_get_size, &ext_sym_render, NULL);
}

GTxt::~GTxt() 
{
}

void GTxt::Init(const std::vector<std::pair<std::string, std::string> >& fonts, 
				const std::vector<std::pair<std::string, std::string> >& user_fonts)
{
	for (int i = 0, n = fonts.size(); i < n; ++i) {
		LoadFont(fonts[i].first.c_str(), fonts[i].second.c_str());
	}
	for (int i = 0, n = user_fonts.size(); i < n; ++i) {
		LoadUserFont(user_fonts[i].first, user_fonts[i].second);
	}
}

void GTxt::LoadFont(const std::string& name, const std::string& filepath)
{
	std::set<std::string>::iterator itr = m_fonts.find(name);
	if (itr != m_fonts.end()) {
		return;
	}

	gtxt_ft_add_font(name.c_str(), filepath.c_str());
	m_fonts.insert(name);
}

void GTxt::LoadUserFont(const std::string& name, const std::string& filepath)
{
	Json::Value value;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, value);
	fin.close();

	int sz = value["chars"].size();

	for (int i = 0, n = value["chars"].size(); i < n; ++i)
	{
		const Json::Value& c_val = value["chars"][i];
		std::string str = c_val["str"].asString();
		std::string utf8 = StringHelper::GBKToUTF8(str);		
		int len = gtxt_unicode_len(utf8[0]);
		int unicode = gtxt_get_unicode(utf8.c_str(), len);
		std::map<int, s2::Symbol*>::iterator itr = m_user_font_chars.find(unicode);
		if (itr != m_user_font_chars.end()) {
			s2::Symbol* sym = SymbolPool::Instance()->Fetch(filepath);
			if (sym) {
				m_user_font_chars.insert(std::make_pair(unicode, sym));
			}
		}
	}
}

void GTxt::LoadUserFontChar(const std::string& str, const std::string& pkg, const std::string& node)
{
	int len = gtxt_unicode_len(str[0]);
	int unicode = gtxt_get_unicode(str.c_str(), len);
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(pkg, node);
	std::map<int, s2::Symbol*>::iterator itr = m_user_font_chars.find(unicode);
	if (itr != m_user_font_chars.end()) {
		if (itr->second->GetID() == id) {
			return;
		} else {
			s2::Symbol* sym = SymbolPool::Instance()->Fetch(id, false);
			if (sym) {
				itr->second->RemoveReference();
				// todo: flatten
				itr->second = sym;
			}
		}
	} else {
		// todo: flatten
		s2::Symbol* sym = SymbolPool::Instance()->Fetch(id, false);
		if (sym) {
			m_user_font_chars.insert(std::make_pair(unicode, sym));
		}
	}
}

void GTxt::AddColor(const std::string& name, unsigned int color)
{
	std::set<std::string>::iterator itr = m_colors.find(name);
	if (itr != m_colors.end()) {
		return;
	}

	gtxt_richtext_add_color(name.c_str(), color);
	m_colors.insert(name);
}

void GTxt::Draw(const gtxt_label_style& style, const S2_MAT& mt, const s2::Color& mul, 
				const s2::Color& add, const std::string& text, int time, bool richtext) const
{
	render_params rp;
	rp.mt = &mt;
	rp.mul = &mul;
	rp.add = &add;

	std::string utf8 = text;
	if (richtext) {
		gtxt_label_draw_richtext(utf8.c_str(), &style, time, (void*)&rp);
	} else {
		gtxt_label_draw(utf8.c_str(), &style, (void*)&rp);
	}
}

void GTxt::Draw(const S2_MAT& mt, const std::string& str, int width) const
{
	if (str.empty()) {
		return;
	}

	gtxt_label_style style;

	style.width = width;
	style.height = 50;

	style.align_h = HA_LEFT;
	style.align_v = VA_CENTER;

	style.space_h = style.space_v = 1;

	style.overflow = true;

	style.gs.font = 0;
	style.gs.font_size = 16;
	style.gs.font_color.integer = 0xffffffff;

	style.gs.edge = true;
	style.gs.edge_size = 1;
	style.gs.edge_color.integer = 0x000000ff;

	render_params rp;
	rp.mt = &mt;
	rp.mul = NULL;
	rp.add = NULL;

	gtxt_label_draw(str.c_str(), &style, (void*)&rp);
}

sm::vec2 GTxt::GetSize(const gtxt_label_style& style, const std::string& text) const
{
	float w, h;
	gtxt_get_label_size(text.c_str(), &style, &w, &h);
	return sm::vec2(w, h);
}

//void GTxt::Reload(const Sprite* spr) 
//{
//	if (spr->GetText().empty()) {
//		return;
//	}
//
//	gtxt_label_style style;
//
//	spr->GetSize(style.width, style.height);
//	spr->GetAlign(style.align_h, style.align_v);
//	spr->GetSpace(style.space_h, style.space_v);
//
//	style.gs.font = spr->GetFont();
//	style.gs.font_size = spr->GetFontSize();
//	style.gs.font_color.integer = color2int(spr->GetFontColor(), PT_RGBA);
//
//	style.gs.edge = spr->GetEdge();
//	style.gs.edge_size = spr->GetEdgeSize();
//	style.gs.edge_color.integer = color2int(spr->GetEdgeColor(), PT_RGBA);
//
//	std::string utf8 = StringHelper::ToUtf8(spr->GetText());
//	gtxt_label_reload_richtext(utf8.c_str(), &style);
//}

void GTxt::Clear()
{
	std::map<int, s2::Symbol*>::iterator itr 
		= m_user_font_chars.begin();
	for ( ; itr != m_user_font_chars.end(); ++itr) {
		itr->second->RemoveReference();
	}
	m_user_font_chars.clear();
}

void GTxt::GetUFLayout(int unicode, int font, struct gtxt_glyph_layout* layout) const
{
	std::map<int, s2::Symbol*>::const_iterator itr = 
		m_user_font_chars.find(unicode);
	if (itr == m_user_font_chars.end()) {
		return;
	}

	sm::rect sz = itr->second->GetBounding();
	layout->sizer.width = sz.Width();
	layout->sizer.height = sz.Height();
	layout->bearing_x = 0;
	layout->bearing_y = 0;
	layout->advance = layout->sizer.width;
	layout->metrics_height = layout->sizer.height;
}

void GTxt::DrawUFChar(int unicode, int font, float x, float y, void* ud) const
{
	std::map<int, s2::Symbol*>::const_iterator itr = 
		m_user_font_chars.find(unicode);
	if (itr == m_user_font_chars.end()) {
		return;
	}

	s2::RenderParams rp;
	render_params* _rp = (render_params*)ud;
	if (_rp->mt) {
		rp.mt = *_rp->mt;
	}
	if (_rp->mul) {
		rp.color.SetMul(*_rp->mul);
	}
	if (_rp->add) {
		rp.color.SetAdd(*_rp->add);
	}
	s2::DrawNode::Draw(itr->second, rp, sm::vec2(x, y));
}

void GTxt::SetCap(int cap_bitmap, int cap_layout)
{
	m_cap_bitmap = cap_bitmap;
	m_cap_layout = cap_layout;
}

}