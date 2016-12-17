#include "SpriteFactory.h"
#include "SymbolPool.h"
#include "Image.h"
#include "GUM_GTxt.h"

#include <render/render.h>
#include <dtex_screen.h>
#include <simp/Package.h>
#include <simp/NodeFactory.h>
#include <gimg_export.h>
#include <gimg_import.h>
#include <sprite2/SprTimer.h>
#include <SM_Matrix.h>

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
	if (id == 0xffffffff) {
		return NULL;
	}
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
void gum_draw_text(const char* str, int x, int y, int w) 
{
	sm::mat4 mat;
	mat.Translate(x, y, 0);
	GTxt::Instance()->Draw(mat, str, w);
}

}