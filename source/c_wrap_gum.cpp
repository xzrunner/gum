#include "SpriteFactory.h"
#include "SymbolPool.h"
#include "Image.h"
#include "GUM_GTxt.h"

#include <render/render.h>
//#include <dtex_screen.h>
#include <simp/Package.h>
#include <simp/NodeFactory.h>
#include <timp/Package.h>
#include <timp/PkgMgr.h>
#include <gimg_export.h>
#include <gimg_import.h>
#include <sprite2/SprTimer.h>
#include <sprite2/RenderCtxStack.h>
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
	const s2::RenderCtx* ctx = s2::RenderCtxStack::Instance()->Top();
	float w = ctx->screen_width;
	float h = ctx->screen_height;

	uint8_t* pixels = (uint8_t*)malloc(w * h * 3);
	memset(pixels, 0, w * h * 3);
	render_gl_read_pixels(0, 0, w, h, pixels);
	gimg_export(filepath, pixels, w, h, GPF_RGB);
	free(pixels);
}

extern "C"
int gum_compare_snapshot(const char* filepath)
{
	const s2::RenderCtx* ctx = s2::RenderCtxStack::Instance()->Top();
	float w = ctx->screen_width;
	float h = ctx->screen_height;

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
bool gum_create_pkg(const char* name, int id, const char* spr_path, const char* tex_path)
{
	simp::Package* s_pkg = new simp::Package(spr_path);
	bool success = simp::NodeFactory::Instance()->AddPkg(s_pkg, name, id);
	if (!success) {
		delete s_pkg;
		return success;
	}

	timp::Package* t_pkg = new timp::Package(tex_path);
	success = timp::PkgMgr::Instance()->Add(t_pkg, id);
	assert(success);

	return true;
}

extern "C"
int gum_pkg_get_page_count(const char* name)
{
	const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(name);
	if (!pkg) {
		return -1;
	}

	return pkg->GetPageCount();
}

extern "C"
void gum_pkg_set_page_filepath(const char* name, int page, const char* filepath)
{
	const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(name);
	if (!pkg) {
		return;
	}

	const_cast<simp::Package*>(pkg)->SetPagePath(page, filepath);
}

extern "C"
void gum_pkg_get_texture_count(int pkg_id, int* tex_count, int* lod_count)
{
	const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	if (!pkg) {
		*tex_count = *lod_count = -1;
		return;
	}

	*tex_count = pkg->GetTexCount();
	*lod_count = pkg->GetLodCount();
}

extern "C"
void gum_pkg_set_texture_filepath(int pkg_id, int tex, int lod, const char* filepath)
{
	const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	if (!pkg) {
		return;
	}

	const_cast<timp::Package*>(pkg)->SetTexPath(tex, lod, filepath);
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