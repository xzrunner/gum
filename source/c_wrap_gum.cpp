#include <string.h>

#include "c_wrap_gum.h"

#include "SpriteFactory.h"
#include "SymbolPool.h"
#include "Image.h"
#include "GUM_GTxt.h"
#include "GUM_DTex.h"
#include "GUM_Sprite2.h"
#include "RenderContext.h"
#include "StringHelper.h"
#include "RenderTargetMgr.h"
#include "RenderTarget.h"
#include "DTexC2Strategy.h"
#include "GUM_Facade.h"
#include "ActorPool.h"
#include "SpritePool.h"
#include "StringHelper.h"

#include <unirender/UR_RenderContext.h>
#include <gimg_typedef.h>
#include <gimg_export.h>
#include <gimg_import.h>
#include <simp/SIMP_Package.h>
#include <simp/NodeFactory.h>
#include <simp/PkgIDMgr.h>
#include <simp/SIMP_Facade.h>
#include <simp/RelocateTexcoords.h>
#include <timp/TIMP_Package.h>
#include <timp/PkgMgr.h>
#include <timp/TIMP_Facade.h>
#include <dtex2/DTEX_Facade.h>
#include <sprite2/SprTimer.h>
#include <sprite2/RenderCtxStack.h>
#include <sprite2/S2_Facade.h>
#include <sprite2/ResetActorFlagVisitor.h>
#include <sprite2/CreateActorsVisitor.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/S2_Actor.h>
#include <sprite2/SprVisitorParams.h>
#include <sprite2/Color.h>
#include <sprite2/ModelSymbol.h>
#include <model3/ModelParametric.h>
#include <shaderlab/SL_Facade.h>
#include <SM_Matrix.h>
#include <dtex2/DTEX_PkgMgr.h>
#include <dtex2/AsyncTask.h>
#include <gum/GUM_AsyncTask.h>
#include <gtxt_label.h>

#include <string.h>

namespace gum
{

extern "C"
void gum_init()
{
	Sprite2::Init();
}

extern "C"
void* gum_get_render_context()
{
	return RenderContext::Instance()->GetImpl();
}

extern "C"
void gum_on_size(int w, int h)
{
	RenderContext::Instance()->OnSize(w, h);
}

extern "C"
void gum_get_screen_size(int* w, int* h)
{
	RenderContext* ctx = RenderContext::Instance();
	*w = ctx->GetWidth();
	*h = ctx->GetHeight();
}

extern "C"
void gum_gc()
{
	ActorPool::Instance()->GC();
	SpritePool::Instance()->GC();
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
	gum::AsyncTask::Instance()->Update();
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
void* gum_create_img(const char* filepath)
{
	return ImageMgr::Instance()->Create(filepath);
}

extern "C"
int gum_get_img_texid(void* img)
{
	Image* gum_img = static_cast<Image*>(img);
	return gum_img->GetTexID();
}

extern "C"
void gum_debug_draw()
{
	//DTex::Instance();
	//DTex::Instance()->DebugDraw();
	//Sprite2::Instance()->DebugDraw();
	//RenderTargetMgr::Instance()->DebugDraw();
}

extern "C"
void gum_clear()
{
	Facade::Clear();

	sl::Facade::Clear();
	s2::Facade::Clear();
	dtex::Facade::Clear();
	timp::Facade::Clear();
	simp::Facade::Clear();

	// todo: clear sprites
}

extern "C"
bool gum_is_async_task_empty()
{
	return dtex::AsyncTask::Instance()->IsEmpty()
		&& AsyncTask::Instance()->IsEmpty();
}

/************************************************************************/
/* simp id                                                              */
/************************************************************************/

extern "C"
void gum_load_pkg_ids(const char* filepath)
{
	simp::PkgIDMgr::Instance()->LoadPkgIDs(filepath);
}

extern "C"
int gum_get_pkg_num()
{
	return simp::PkgIDMgr::Instance()->GetPkgSize();
}

extern "C"
void gum_get_all_pkg_names(const char* names[])
{
	simp::PkgIDMgr::Instance()->GetAllPkgNames(names);
}

extern "C"
int gum_query_pkg_id(const char* name)
{
	std::string gbk_name = StringHelper::UTF8ToGBK(name);
	return simp::PkgIDMgr::Instance()->QueryPkgID(gbk_name);
}

/************************************************************************/
/* pkg                                                                  */
/************************************************************************/

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
void gum_release_pkg(int pkg_id)
{
	simp::RelocateTexcoords::Instance()->Delete(pkg_id);
	timp::PkgMgr::Instance()->Delete(pkg_id);
	dtex::PkgMgr::Instance()->Delete(pkg_id);
}

extern "C"
void gum_pkg_set_release_tag()
{
	simp::RelocateTexcoords::Instance()->SetReleaseTag();
}

extern "C"
void gum_pkg_release_after_last_tag()
{
	simp::RelocateTexcoords::Instance()->ReleaseAfterLastTag();
	DTex::Instance()->ClearSymbolCache();
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

/************************************************************************/
/* symbol                                                               */
/************************************************************************/

extern "C"
void* gum_create_sym_model(const void* surface)
{
	const m3::Surface* m3_surface = static_cast<const m3::Surface*>(surface);
	m3::AABB aabb;
	m3::Model* model = new m3::ModelParametric(m3_surface, aabb);
	s2::ModelSymbol* sym = new s2::ModelSymbol();
	sym->SetModel(model);
	return sym;
}

/************************************************************************/
/* sprite                                                               */
/************************************************************************/

extern "C"
void* gum_create_spr(const char* pkg, const char* spr)
{
	std::string gbk_pkg = StringHelper::UTF8ToGBK(pkg);
	std::string gbk_spr = StringHelper::UTF8ToGBK(spr);
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(gbk_pkg, gbk_spr);
	if (id == 0xffffffff) {
		return NULL;
	} else {
		return gum_create_spr_by_id(id);
	}
}

extern "C"
void* gum_create_spr_by_id(int id)
{
	return SpriteFactory::Instance()->CreateFromSym(id, true);
}

extern "C"
void* gum_create_spr_from_file(const char* filepath)
{
	std::string gbk_filepath = StringHelper::UTF8ToGBK(filepath);
	return SpriteFactory::Instance()->Create(gbk_filepath);
}

extern "C"
void* gum_fetch_actor_cached(const char* pkg, const char* spr, bool* is_new)
{
	std::string gbk_pkg = StringHelper::UTF8ToGBK(pkg);
	std::string gbk_spr = StringHelper::UTF8ToGBK(spr);
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(gbk_pkg, gbk_spr);
	if (id == 0xffffffff) {
		return NULL;
	} else {
		return gum::ActorPool::Instance()->Fetch(id, *is_new);
	}
}

extern "C"
void gum_return_actor_cached(void* actor)
{
	s2::Actor* s2_actor = static_cast<s2::Actor*>(actor);
	ActorPool::Instance()->Return(s2_actor);
	s2_actor->GetSpr()->RemoveReference();
}

extern "C"
void gum_draw_text(const char* str, int x, int y, int w) 
{
	std::string gbk_str = StringHelper::UTF8ToGBK(str);

	S2_MAT mat;
	mat.Translate(x, y);
	GTxt::Instance()->Draw(mat, gbk_str, w);
}

/************************************************************************/
/* rt                                                                   */
/************************************************************************/

extern "C"
void* gum_rt_fetch()
{
	return RenderTargetMgr::Instance()->Fetch();
}

extern "C"
void gum_rt_return(void* rt)
{
	RenderTarget* gum_rt = static_cast<RenderTarget*>(rt);
	RenderTargetMgr::Instance()->Return(gum_rt);
}

extern "C"
void gum_rt_bind(void* rt)
{
	static_cast<RenderTarget*>(rt)->Bind();
}

extern "C"
void gum_rt_unbind(void* rt)
{
	static_cast<RenderTarget*>(rt)->Unbind();
}

extern "C"
void gum_rt_draw(void* rt, struct gum_region* src)
{
	RenderTarget* gum_rt = static_cast<RenderTarget*>(rt);
	if (src) {
		gum_rt->Draw(src->xmin, src->ymin, src->xmax, src->ymax);
	} else {
		gum_rt->Draw();
	}
}

extern "C"
int gum_rt_get_texid(void* rt)
{
	return static_cast<RenderTarget*>(rt)->GetTexID();
}

/************************************************************************/
/* dtex                                                                 */
/************************************************************************/

extern "C"
void gum_dtex_set_c2_max_edge(int max_edge)
{
	DTexC2Strategy::Instance()->SetMaxC2Edge(max_edge);
}

/************************************************************************/
/* gtxt                                                                 */
/************************************************************************/

extern "C"
void gum_gtxt_init(int cap_bitmap, int cap_layout)
{
	GTxt::SetCap(cap_bitmap, cap_layout);
	GTxt::Instance();
}

extern "C"
void gum_gtxt_clear()
{
	GTxt::Instance()->Clear();
}

extern "C"
void gum_gtxt_add_font(const char* name, const char* path)
{
	GTxt::Instance()->LoadFont(name, path);
}

extern "C"
void gum_gtxt_add_color(const char* name, unsigned int color)
{
	GTxt::Instance()->AddColor(name, color);
}

extern "C"
void gum_gtxt_add_user_font_char(const char* str, const char* pkg, const char* node)
{
	GTxt::Instance()->LoadUserFontChar(str, pkg, node);
}

extern "C"
void gum_gtxt_print(const char* str, float x, float y, int font_size, uint32_t font_color)
{
	gtxt_label_style s;

	s.width					= 200;
	s.height				= 200;

	s.gs.font				= 0;
	s.gs.font_size			= font_size;
	s.gs.font_color.integer = font_color;

	s.gs.edge				= false;
	s.gs.edge_size			= 0;
	s.gs.edge_color.integer = 0xffffffff;

	s.align_h				= HA_CENTER;
	s.align_v				= VA_CENTER;

	s.space_h				= 1;
	s.space_v				= 1;

	s.overflow				= true;

	S2_MAT mt;
	mt.Translate(x, y);
	GTxt::Instance()->Draw(s, mt, s2::Color(255, 255, 255, 255), s2::Color(0, 0, 0, 0), StringHelper::FromChar(str), 0, false);
}

extern "C"
void gum_gtxt_size(const char* str, int font_size, float* w, float* h)
{
	gtxt_label_style s;

	s.width					= 200;
	s.height				= 200;

	s.gs.font				= 0;
	s.gs.font_size			= font_size;
	s.gs.font_color.integer = 0xffffffff;

	s.gs.edge				= false;
	s.gs.edge_size			= 0;
	s.gs.edge_color.integer = 0xffffffff;

	s.align_h				= HA_CENTER;
	s.align_v				= VA_CENTER;

	s.space_h				= 1;
	s.space_v				= 1;

	s.overflow				= true;

	sm::vec2 sz = GTxt::Instance()->GetSize(s, StringHelper::FromChar(str));
	*w = sz.x;
	*h = sz.y;
}

}
