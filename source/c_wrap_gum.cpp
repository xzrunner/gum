#include "SpriteFactory.h"
#include "SymbolPool.h"
#include "Image.h"

#include <render/render.h>
#include <dtex_screen.h>
#include <simp/Package.h>
#include <simp/NodeFactory.h>
#include <gimg_export.h>
#include <gimg_import.h>
#include <sprite2/DrawNode.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/SprTimer.h>
#include <sprite2/ComplexSprite.h>
#include <sprite2/AnimSprite.h>
#include <sprite2/AnimSymbol.h>
#include <sprite2/ComplexSymbol.h>
#include <sprite2/TextboxSprite.h>
#include <sprite2/RenderFilter.h>
#include <sprite2/BoundingBox.h>
#include <sprite2/S2_RVG.h>
#include <sprite2/PointQueryVisitor.h>
#include <c_wrap_sl.h>
#include <gtxt_label.h>

namespace gum
{

extern "C"
void gum_gc()
{
	SymbolPool::Instance()->GC();
	ImageMgr::Instance()->GC();
}

extern "C"
int gum_get_sym_count() {
	return SymbolPool::Instance()->Count();
}

extern "C"
int gum_get_img_count() {
	return ImageMgr::Instance()->Count();
}

extern "C"
int gum_get_spr_count() {
	return s2::Sprite::GetCount();
}

extern "C"
void gum_update(float dt)
{
	s2::SprTimer::Instance()->Update(dt);
}

extern "C"
void gum_store_snapshot(const char* filepath)
{
	float w, h, scale;
	dtex_get_screen(&w, &h, &scale);
	w *= scale;
	h *= scale;

	uint8_t* pixels = (uint8_t*)malloc(w * h * 3);
	memset(pixels, 0, w * h * 3);
	render_gl_read_pixels(0, 0, w, h, pixels);
	gimg_export(filepath, pixels, w, h, GPF_RGB);
	free(pixels);
}

extern "C"
int gum_compare_snapshot(const char* filepath)
{
	float w, h, scale;
	dtex_get_screen(&w, &h, &scale);
	w *= scale;
	h *= scale;

	int sz = w * h * 3;
	uint8_t* now = (uint8_t*)malloc(sz);
	memset(now, 0, sz);
	render_gl_read_pixels(0, 0, w, h, now);

	int _w, _h;
	enum GIMG_PIXEL_FORMAT _fmt;
	uint8_t* old = gimg_import(filepath, &_w, &_h, &_fmt);

	int cmp = 0;
	if (_w != static_cast<int>(w) || _h != static_cast<int>(h)) {
		cmp = -1;
	} else {
		cmp = memcmp(old, now, sz);
	}

	free(now);
	free(old);

	return cmp;
}

extern "C"
bool gum_create_pkg(const char* filepath, const char* name, int id)
{
	simp::Package* pkg = new simp::Package(filepath);
	return simp::NodeFactory::Instance()->AddPkg(pkg, name, id);
}

extern "C"
void* gum_create_spr(const char* pkg, const char* spr)
{
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(pkg, spr);
	return SpriteFactory::Instance()->CreateFromSym(id);
}

extern "C"
void* gum_create_spr_by_id(int id)
{
	return SpriteFactory::Instance()->CreateFromSym(id);
}

extern "C"
void* gum_create_spr_from_file(const char* filepath)
{
	return SpriteFactory::Instance()->Create(filepath);
}

extern "C"
void gum_spr_draw(const void* spr, float x, float y, float angle, float sx, float sy)
{
	s2::RenderParams params;
	params.mt.SetTransformation(x, y, angle, sx, sy, 0, 0, 0, 0);
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	s2::DrawNode::Draw(s2_spr, params);
}

extern "C"
void gum_spr_update(void* spr) {
	static_cast<s2::Sprite*>(spr)->Update(s2::RenderParams());
}

extern "C"
void gum_spr_release(void* spr) {
	s2::Sprite* test = static_cast<s2::Sprite*>(spr);
	test->RemoveReference();
}

extern "C"
void* gum_spr_fetch_child(const void* spr, const char* name) {
	const s2::Sprite* child = static_cast<const s2::Sprite*>(spr)->FetchChild(name);
	if (child) {
		child->AddReference();
		return const_cast<s2::Sprite*>(child);
	} else {
		return NULL;
	}
}

extern "C"
void* gum_spr_fetch_child_by_index(const void* spr, int idx) {
	const s2::Sprite* child = static_cast<const s2::Sprite*>(spr)->FetchChild(idx);
	if (child) {
		child->AddReference();
		return const_cast<s2::Sprite*>(child);
	} else {
		return NULL;
	}
}

extern "C"
void gum_spr_mount_child(void* parent, const char* name, void* child) {
	static_cast<s2::Sprite*>(parent)->MountChild(name, static_cast<s2::Sprite*>(child));
}

extern "C"
void gum_spr_set_pos(void* spr, float x, float y) {
	static_cast<s2::Sprite*>(spr)->SetPosition(sm::vec2(x, y));
}

extern "C"
void gum_spr_set_angle(void* spr, float angle) {
	static_cast<s2::Sprite*>(spr)->SetAngle(angle);
}

extern "C"
void gum_spr_set_scale(void* spr, float sx, float sy) {
	static_cast<s2::Sprite*>(spr)->SetScale(sm::vec2(sx, sy));
}

extern "C"
void gum_spr_get_pos(const void* spr, float* x, float* y) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	*x = s2_spr->GetPosition().x;
	*y = s2_spr->GetPosition().y;
}

extern "C"
void gum_spr_get_angle(const void* spr, float* angle) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	*angle = s2_spr->GetAngle();
}

extern "C"
void gum_spr_get_scale(const void* spr, float* sx, float* sy) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	*sx = s2_spr->GetScale().x;
	*sy = s2_spr->GetScale().y;
}

extern "C"
int gum_spr_get_sym_id(void* spr) {
	return static_cast<s2::Sprite*>(spr)->GetSymbol()->GetID();
}

extern "C"
const char* gum_spr_get_name(void* spr) {
	return static_cast<s2::Sprite*>(spr)->GetName().c_str();
}

extern "C"
int gum_spr_get_sym_type(void* spr) {
	return static_cast<s2::Sprite*>(spr)->GetSymbol()->Type();
}

extern "C"
bool gum_spr_get_visible(void* spr) {
	return static_cast<s2::Sprite*>(spr)->IsVisible();
}

extern "C"
void gum_spr_set_visible(void* spr, bool visible) {
	static_cast<s2::Sprite*>(spr)->SetVisible(visible);
}

extern "C"
bool gum_spr_get_editable(void* spr) {
	return static_cast<s2::Sprite*>(spr)->IsEditable();	
}

extern "C"
void gum_spr_set_editable(void* spr, bool editable) {
	static_cast<s2::Sprite*>(spr)->SetEditable(editable);
}

extern "C"
void gum_spr_set_frame(void* spr, int frame) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_ANIMATION) {
		s2::AnimSprite* anim = VI_DOWNCASTING<s2::AnimSprite*>(s2_spr);
		anim->SetFrame(frame);
	}
}

extern "C"
void gum_spr_set_action(void* spr, const char* action) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_COMPLEX) {
		s2::ComplexSprite* complex = VI_DOWNCASTING<s2::ComplexSprite*>(s2_spr);
		complex->SetAction(action);
	}
}

extern "C"
int gum_spr_get_frame(void* spr) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_ANIMATION) {
		s2::AnimSprite* anim = VI_DOWNCASTING<s2::AnimSprite*>(s2_spr);
		return anim->GetFrame();
	} else {
		return -1;
	}
}

extern "C"
int gum_spr_get_frame_count(void* spr) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_ANIMATION) {
		s2::AnimSprite* anim = VI_DOWNCASTING<s2::AnimSprite*>(s2_spr);
		const s2::AnimSymbol* sym = VI_DOWNCASTING<const s2::AnimSymbol*>(anim->GetSymbol());
		return sym->GetMaxFrameIdx();
	} else {
		return -1;
	}
}

extern "C"
int gum_spr_get_component_count(void* spr) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_COMPLEX) {
		s2::ComplexSprite* complex = VI_DOWNCASTING<s2::ComplexSprite*>(s2_spr);
		const s2::ComplexSymbol* sym = VI_DOWNCASTING<const s2::ComplexSymbol*>(complex->GetSymbol());
		return sym->GetChildren().size();
	} else {
		return -1;
	}
}

extern "C"
uint32_t gum_spr_get_col_mul(void* spr) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	return s2_spr->GetColor().mul.ToRGBA();
}

extern "C"
uint32_t gum_spr_get_col_add(void* spr) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	return s2_spr->GetColor().add.ToRGBA();
}

extern "C"
void gum_spr_get_col_map(void* spr, uint32_t* rmap, uint32_t* gmap, uint32_t* bmap) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	*rmap = s2_spr->GetColor().rmap.ToRGBA();
	*gmap = s2_spr->GetColor().gmap.ToRGBA();
	*bmap = s2_spr->GetColor().bmap.ToRGBA();
}

extern "C"
void gum_spr_set_col_mul(void* spr, uint32_t rgba) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetColor().mul.ToRGBA() == rgba) {
		return;
	}

	s2::RenderColor col = s2_spr->GetColor();
	col.mul.FromRGBA(rgba);
	s2_spr->SetColor(col);
}

extern "C"
void gum_spr_set_col_add(void* spr, uint32_t rgba) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetColor().add.ToRGBA() == rgba) {
		return;
	}

	s2::RenderColor col = s2_spr->GetColor();
	col.add.FromRGBA(rgba);
	s2_spr->SetColor(col);
}

extern "C"
void gum_spr_set_col_map(void* spr, uint32_t rmap, uint32_t gmap, uint32_t bmap) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetColor().rmap.ToRGBA() == rmap &&
		s2_spr->GetColor().gmap.ToRGBA() == gmap &&
		s2_spr->GetColor().bmap.ToRGBA() == bmap) {
		return;
	}

	s2::RenderColor col = s2_spr->GetColor();
	col.rmap.FromRGBA(rmap);
	col.gmap.FromRGBA(gmap);
	col.bmap.FromRGBA(bmap);
	s2_spr->SetColor(col);
}

extern "C"
void gum_spr_set_filter(void* spr, int mode) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	const s2::RenderFilter* filter = s2_spr->GetShader().GetFilter();
	int ori = SLFM_NULL;
	if (filter) {
		ori = filter->GetMode();
	}
	if (ori == mode) {
		return;
	}

	s2::RenderShader shader = s2_spr->GetShader();
	shader.SetFilter(s2::FilterMode(mode));
	s2_spr->SetShader(shader);
}

extern "C"
const char* gum_spr_get_text(void* spr) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_TEXTBOX) {
		s2::TextboxSprite* text = VI_DOWNCASTING<s2::TextboxSprite*>(s2_spr);
		return text->GetText().c_str();
	} else {
		return NULL;
	}
}

extern "C"
void gum_spr_set_text(void* spr, const char* text) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_TEXTBOX) {
		s2::TextboxSprite* text_spr = VI_DOWNCASTING<s2::TextboxSprite*>(s2_spr);
		text_spr->SetText(text);
	}
}

extern "C"
void gum_spr_get_aabb(const void* spr, float aabb[4]) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	sm::rect sz = s2_spr->GetBounding()->GetSize();
	aabb[0] = sz.xmin;
	aabb[1] = sz.ymin;
	aabb[2] = sz.xmax;
	aabb[3] = sz.ymax;
}

extern "C"
void gum_spr_draw_aabb(const void* spr, float x, float y, float angle, float sx, float sy, const float mat[6]) {
	sm::mat4 outer;
 	outer.x[0] = mat[0];
 	outer.x[1] = mat[1];
 	outer.x[4] = mat[2];
 	outer.x[5] = mat[3];
 	outer.x[12]= mat[4];
 	outer.x[13]= mat[5];

	sm::mat4 m;
	m.SetTransformation(x, y, angle, sx, sy, 0, 0, 0, 0);
	m = outer * m;

	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	sm::rect sz = s2_spr->GetSymbol()->GetBounding(s2_spr);
	std::vector<sm::vec2> vertices(4);
	vertices[0] = sm::vec2(sz.xmin, sz.ymin);
	vertices[1] = sm::vec2(sz.xmin, sz.ymax);
	vertices[2] = sm::vec2(sz.xmax, sz.ymax);
	vertices[3] = sm::vec2(sz.xmax, sz.ymin);
	for (int i = 0; i < 4; ++i) {
		vertices[i] = m * vertices[i];
	}
	s2::RVG::Polyline(vertices, true);
}

extern "C"
bool gum_spr_point_test(const void* spr, float x, float y) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	return s2_spr->GetBounding()->IsContain(sm::vec2(x, y));
}

extern "C"
void* gum_spr_point_query(const void* spr, float x, float y, float mat[6]) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);

	s2::PointQueryVisitor visitor(sm::vec2(x, y));
	sm::mat4 _mat;
	s2_spr->Traverse(visitor, &_mat);
	const s2::Sprite* ret = visitor.GetSelectedSpr();
	if (!ret) {
		return NULL;
	}

	_mat = visitor.GetSelectedMat();
 	mat[0] = _mat.x[0];
 	mat[1] = _mat.x[1];
 	mat[2] = _mat.x[4];
 	mat[3] = _mat.x[5];
 	mat[4] = _mat.x[12];
 	mat[5] = _mat.x[13];

	ret->AddReference();
	return const_cast<s2::Sprite*>(ret);
}

extern "C"
bool gum_spr_has_action(const void* spr, const char* name) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	const s2::ComplexSymbol* sym = static_cast<const s2::ComplexSymbol*>(s2_spr->GetSymbol());
	const std::vector<s2::ComplexSymbol::Action>& actions = sym->GetActions();
	for (int i = 0, n = actions.size(); i < n; ++i) {
		if (name == actions[i].name) {
			return true;
		}
	}
	return false;
}

static void
_init_gtxt_label_style(struct gtxt_label_style* dst, const s2::Textbox& src) {
	dst->width  = src.width;
	dst->height = src.height;

	dst->align_h = src.align_hori;
	dst->align_v = src.align_vert;

	dst->space_h = src.space_hori;
	dst->space_v = src.space_vert;

	dst->gs.font = src.font_type;
	dst->gs.font_size = src.font_size;
	dst->gs.font_color.integer = src.font_color.ToRGBA();

	dst->gs.edge = src.has_edge;
	dst->gs.edge_size = src.edge_size;
	dst->gs.edge_color.integer = src.edge_color.ToRGBA();

	dst->overflow = src.overflow;
}

extern "C"
bool gum_spr_get_text_size(const void* spr, float* w, float* h) {
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() != s2::SYM_TEXTBOX) {
		return false;
	}

	const s2::TextboxSprite* text_spr = static_cast<const s2::TextboxSprite*>(s2_spr);

	struct gtxt_label_style style;
	_init_gtxt_label_style(&style, text_spr->GetTextbox());
	gtxt_get_label_size(text_spr->GetText().c_str(), &style, w, h);	

	return true;
}

extern "C"
bool gum_spr_get_scissor(const void* spr, float* x, float* y, float* w, float* h)
{
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() != s2::SYM_COMPLEX) {
		return false;
	}

	const s2::ComplexSprite* complex_spr = static_cast<const s2::ComplexSprite*>(s2_spr);
	const s2::ComplexSymbol* sym = static_cast<const s2::ComplexSymbol*>(complex_spr->GetSymbol());
	const sm::rect& scissor = sym->GetScissor();
	*x = scissor.xmin;
	*y = scissor.ymin;
	*w = scissor.xmax - scissor.xmin;
	*h = scissor.ymax - scissor.ymin;

	return true;
}

extern "C"
bool gum_spr_set_scissor(void* spr, float x, float y, float w, float h)
{
	const s2::Sprite* s2_spr = static_cast<const s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() != s2::SYM_COMPLEX) {
		return false;
	}

	const s2::ComplexSprite* complex_spr = static_cast<const s2::ComplexSprite*>(s2_spr);
	const s2::ComplexSymbol* sym = static_cast<const s2::ComplexSymbol*>(complex_spr->GetSymbol());
	sm::rect scissor;
	scissor.xmin = x;
	scissor.ymin = y;
	scissor.xmax = x + w;
	scissor.ymax = y + h;

	return true;
}

}