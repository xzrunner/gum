#include "SpriteFactory.h"
#include "SymbolPool.h"
#include "Image.h"
#include "GUM_GTxt.h"
#include "GUM_DTex.h"
#include "RenderContext.h"
#include "StringHelper.h"

#include <unirender/RenderContext.h>
#include <gimg_typedef.h>
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
void* gum_get_render_context()
{
	return RenderContext::Instance()->GetImpl();
}

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
void gum_flush()
{
	DTex::Instance()->Flush();
}

extern "C"
void gum_store_snapshot(const char* filepath)
{
	std::string gbk_filepath = StringHelper::UTF8ToGBK(filepath);

	const s2::RenderContext* ctx = s2::RenderCtxStack::Instance()->Top();
	float w = ctx->GetScreenWidth();
	float h = ctx->GetScreenHeight();

	uint8_t* pixels = (uint8_t*)malloc(w * h * 3);
	memset(pixels, 0, w * h * 3);
	RenderContext::Instance()->GetImpl()->ReadPixels(pixels, 3, 0, 0, w, h);
	gimg_export(gbk_filepath.c_str(), pixels, w, h, GPF_RGB, true);
	free(pixels);
}

extern "C"
int gum_compare_snapshot(const char* filepath)
{
	std::string gbk_filepath = StringHelper::UTF8ToGBK(filepath);

	const s2::RenderContext* ctx = s2::RenderCtxStack::Instance()->Top();
	float w = ctx->GetScreenWidth();
	float h = ctx->GetScreenHeight();

	int sz = w * h * 3;
	uint8_t* now = (uint8_t*)malloc(sz);
	memset(now, 0, sz);
	RenderContext::Instance()->GetImpl()->ReadPixels(now, 3, 0, 0, w, h);

	int _w, _h;
	int _fmt;
	uint8_t* old = gimg_import(gbk_filepath.c_str(), &_w, &_h, &_fmt);

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
bool gum_pkg_exists(const char* name)
{
	std::string gbk_name = StringHelper::UTF8ToGBK(name);
	return simp::NodeFactory::Instance()->QueryPkg(gbk_name) != NULL;
}

extern "C"
bool gum_create_pkg(const char* name, int id, const char* spr_path, const char* tex_path)
{
	std::string gbk_name = StringHelper::UTF8ToGBK(name);
	std::string gbk_spr_path = StringHelper::UTF8ToGBK(spr_path);
	std::string gbk_tex_path = StringHelper::UTF8ToGBK(tex_path);

	simp::Package* s_pkg = new simp::Package(gbk_spr_path, id);
	bool success = simp::NodeFactory::Instance()->AddPkg(s_pkg, gbk_name, id);
	if (!success) {
		delete s_pkg;
		return success;
	}

	timp::Package* t_pkg = new timp::Package(gbk_tex_path);
	success = timp::PkgMgr::Instance()->Add(t_pkg, id);
	assert(success);

	DTex::Instance()->CreatePkg(id);

	return true;
}

extern "C"
int gum_pkg_get_page_count(const char* name)
{
	std::string gbk_name = StringHelper::UTF8ToGBK(name);
	const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(gbk_name);
	if (!pkg) {
		return -1;
	}

	return pkg->GetPageCount();
}

extern "C"
void gum_pkg_set_page_filepath(const char* name, int page, const char* filepath)
{
	std::string gbk_name = StringHelper::UTF8ToGBK(name);
	std::string gbk_filepath = StringHelper::UTF8ToGBK(filepath);
	const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(gbk_name);
	if (!pkg) {
		return;
	}

	const_cast<simp::Package*>(pkg)->SetPagePath(page, gbk_filepath);
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
	std::string gbk_filepath = StringHelper::UTF8ToGBK(filepath);

	const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	if (!pkg) {
		return;
	}

	const_cast<timp::Package*>(pkg)->SetTexPath(tex, lod, gbk_filepath);
}

extern "C"
void* gum_create_spr(const char* pkg, const char* spr)
{
	std::string gbk_pkg = StringHelper::UTF8ToGBK(pkg);
	std::string gbk_spr = StringHelper::UTF8ToGBK(spr);

	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(gbk_pkg, gbk_spr);
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
	std::string gbk_filepath = StringHelper::UTF8ToGBK(filepath);
	return SpriteFactory::Instance()->Create(gbk_filepath);
}

extern "C"
void gum_init_gtxt(int cap_bitmap, int cap_layout)
{
	GTxt::SetCap(cap_bitmap, cap_layout);
	GTxt::Instance();
}

extern "C"
void gum_draw_text(const char* str, int x, int y, int w) 
{
	std::string gbk_str = StringHelper::UTF8ToGBK(str);

	sm::mat4 mat;
	mat.Translate(x, y, 0);
	GTxt::Instance()->Draw(mat, gbk_str, w);
}

extern "C"
void gum_debug_draw()
{
	DTex::Instance()->DebugDraw();
}

}