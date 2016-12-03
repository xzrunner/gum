#include "SpriteFactory.h"
#include "SymbolPool.h"

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
#include <sprite2/RenderFilter.h>
#include <c_wrap_sl.h>

namespace gum
{

extern "C"
void gum_gc()
{
	SymbolPool::Instance()->GC();
}

extern "C"
int gum_get_sym_count() {
	return SymbolPool::Instance()->Count();
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
void gum_store_snapshoot(const char* filepath)
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
int gum_compare_snapshoot(const char* filepath)
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
void gum_create_pkg(const char* filepath, const char* name, int id)
{
	simp::Package* pkg = new simp::Package(filepath);
	simp::NodeFactory::Instance()->AddPkg(pkg, name, id);
}

extern "C"
void* gum_create_spr(const char* pkg, const char* spr)
{
	uint32_t id = simp::NodeFactory::Instance()->GetID(pkg, spr);
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
	s2::DrawNode::Draw(static_cast<const s2::Sprite*>(spr), params);
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
void* gum_fetch_child(const void* spr, const char* name) {
	return static_cast<const s2::Sprite*>(spr)->FetchChild(name);
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
void gum_spr_set_action(void* spr, const char* action) {
	s2::Sprite* s2_spr = static_cast<s2::Sprite*>(spr);
	if (s2_spr->GetSymbol()->Type() == s2::SYM_COMPLEX) {
		s2::ComplexSprite* complex = VI_DOWNCASTING<s2::ComplexSprite*>(s2_spr);
		complex->SetAction(action);
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

}