#include <string.h>

#include "c_wrap_gum.h"

#include "gum/SpriteFactory.h"
#include "gum/SymbolPool.h"
#include "gum/Image.h"
#include "gum/ImagePool.h"
#include "gum/GTxt.h"
#include "gum/DTex.h"
#include "gum/Sprite2.h"
#include "gum/RenderContext.h"
#include "gum/StringHelper.h"
#include "gum/RenderTargetMgr.h"
#include "gum/RenderTarget.h"
#include "gum/DTexC2Strategy.h"
#include "gum/Facade.h"
#include "gum/ActorPool.h"
#include "gum/SpritePool.h"
#include "gum/StringHelper.h"
#include "gum/Statistics.h"
#include "gum/StatFPS.h"
#include "gum/StatTag.h"
#include "gum/StatScreen.h"
#include "gum/StatTasks.h"
#include "gum/PkgFileParser.h"
#ifndef S2_DISABLE_MODEL
#include "gum/Model3.h"
#endif // S2_DISABLE_MODEL
#include "gum/Audio.h"
#include "gum/LoadImageTask.h"
#include "gum/Audio.h"
#include "gum/Cooking.h"
#include "gum/ShaderLab.h"

#include <unirender/RenderContext.h>
#include <uniaudio/AudioContext.h>
#include <gimg_typedef.h>
#include <gimg_export.h>
#include <gimg_import.h>
#include <simp/Package.h>
#include <simp/NodeFactory.h>
#include <simp/PkgIDMgr.h>
#include <simp/AudioIDMgr.h>
#include <simp/Facade.h>
#include <simp/RelocateTexcoords.h>
#include <timp/Package.h>
#include <timp/PkgMgr.h>
#include <timp/Facade.h>
#include <dtex2/Facade.h>
#include <dtex2/LoadResTask.h>
#include <sprite2/SprTimer.h>
#include <sprite2/RenderCtxStack.h>
#include <sprite2/Facade.h>
#include <sprite2/ResetActorFlagVisitor.h>
#include <sprite2/CreateActorsVisitor.h>
#include <sprite2/Sprite.h>
#include <sprite2/Actor.h>
#include <sprite2/SprVisitorParams.h>
#include <sprite2/Color.h>
#include <sprite2/ModelSymbol.h>
#include <sprite2/StatImages.h>
#include <sprite2/Blackboard.h>
#include <sprite2/ActorFactory.h>
#include <sprite2/ActorProxy.h>
#include <sprite2/Symbol.h>
#include <sprite2/DrawTask.h>
#include <shaderlab/Facade.h>
#include <sprite2/TextTable.h>
#include <SM_Matrix.h>
#include <logger.h>
#include <dtex2/PkgMgr.h>
#include <gtxt_label.h>
#include <c_wrap_dtex.h>
#include <c_wrap_cooking.h>
#include <multitask/TaskStat.h>

#include <queue>

#include <string.h>
#include <fault.h>

namespace gum
{

extern "C"
void gum_init(void (*error_reload)(), void* arg1, void* arg2)
{
	try {
		DTex::Instance()->InitHook(nullptr, nullptr, error_reload);

		Sprite2::Instance()->Init();

		ShaderLab::Init();

		Cooking::Init();

		Audio::Instance()->Initialize(arg1, arg2);

#ifndef S2_DISABLE_MODEL
		Model3::Instance();
#endif // S2_DISABLE_MODEL
	} catch (std::exception& e) {
		fault("gum_init fail: %s\n", e.what());		
	}
}

extern "C"
void gum_on_pause() {
	Audio* audio = Audio::Instance();
	if (audio->IsEnable()) {
		audio->GetContext()->Pause();
	}
}

extern "C"
void gum_on_resume() {
	Audio* audio = Audio::Instance();
	if (audio->IsEnable()) {
		audio->GetContext()->Resume();
	}
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
	ImagePool::Instance()->GC();

	DTexC2Strategy::Instance()->Clear();
}

extern "C"
int gum_get_sym_count() {
	return s2::Symbol::GetAllSymCount();
}

extern "C"
int gum_get_spr_count()
{
	return s2::Sprite::GetAllSprCount();
}

extern "C"
int gum_get_actor_count()
{
	return s2::Actor::GetAllActorCount();
}

extern "C"
int gum_get_img_count() {
	return Image::GetAllImgCount();
}

extern "C"
void gum_update(float dt)
{
	s2::SprTimer::Instance()->Update(dt);
}

extern "C"
void gum_flush()
{
	dtex::Facade::Flush();
	DTex::Instance()->Flush();
	LoadImageTaskMgr::Instance()->Flush();
	Sprite2::Instance()->Flush();
}

extern "C"
void  gum_flush_deferred()
{
#ifndef S2_DISABLE_DEFERRED
	{
		int type = mt::TaskStat::Instance()->RegisterTaskType("wait");
		mt::TaskStat::Checkpoint cp(std::this_thread::get_id(), type);

		// wait all task finished
		s2::DrawTaskMgr* task_mgr = s2::DrawTaskMgr::Instance();
		while (!task_mgr->IsAllTaskFinished())
			;
	}
	{
		int type = mt::TaskStat::Instance()->RegisterTaskType("commit");
		mt::TaskStat::Checkpoint cp(std::this_thread::get_id(), type);

		s2::DrawTaskMgr::Instance()->Flush();
	}
#endif // S2_DISABLE_DEFERRED
}

extern "C"
void gum_store_snapshot(const char* filepath)
{
	CU_STR gbk_filepath = StringHelper::UTF8ToGBK(filepath);

	const s2::RenderContext* ctx = s2::RenderCtxStack::Instance()->Top();
	int w = ctx->GetScreenWidth();
	int h = ctx->GetScreenHeight();

	gum::StringHelper::ToLower(gbk_filepath);
	if (gbk_filepath.find("jpg") != std::string::npos)
	{
		// channel 3 has some problem on ios
		static int channel = 4;

		uint8_t* rgba = (uint8_t*)malloc(w * h * channel);
		memset(rgba, 0, w * h * channel);
		RenderContext::Instance()->GetImpl()->ReadPixels(rgba, channel, 0, 0, w, h);

		uint8_t* rgb = gimg_rgba2rgb(rgba, w, h);
		free(rgba);
		gimg_export(gbk_filepath.c_str(), rgb, w, h, GPF_RGB, true);
		free(rgb);
	}
	else
	{
		uint8_t* pixels = (uint8_t*)malloc(w * h * 4);
		memset(pixels, 0, w * h * 4);
		RenderContext::Instance()->GetImpl()->ReadPixels(pixels, 4, 0, 0, w, h);
		gimg_export(gbk_filepath.c_str(), pixels, w, h, GPF_RGBA8, true);
		free(pixels);
	}
}

extern "C"
int gum_compare_snapshot(const char* filepath)
{
	CU_STR gbk_filepath = StringHelper::UTF8ToGBK(filepath);

	const s2::RenderContext* ctx = s2::RenderCtxStack::Instance()->Top();
	int w = ctx->GetScreenWidth();
	int h = ctx->GetScreenHeight();

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

static std::vector<std::shared_ptr<Image>> IMAGE_CACHE;

extern "C"
void* gum_create_img(const char* filepath)
{
	bimp::FilePath res_path(filepath);
	auto img = ImagePool::Instance()->Create(s2::StatImages::UNKNOWN_IMG_ID, res_path);
	IMAGE_CACHE.push_back(img);
	return static_cast<void*>(img.get());
}

extern "C"
int gum_get_img_texid(void* img)
{
	Image* gum_img(static_cast<Image*>(img));
	return gum_img->GetTexID();
}

extern "C"
void gum_debug_draw()
{
	//DTex::Instance();
	//DTex::Instance()->DebugDraw();
	//Sprite2::Instance()->DebugDraw();
	//RenderTargetMgr::Instance()->DebugDraw();
	//StatScreen::Instance()->DebugDraw();
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
	return dtex::LoadResTaskMgr::Instance()->IsEmpty()
		&& LoadImageTaskMgr::Instance()->IsEmpty();
}

extern "C"
bool gum_is_support_etc2()
{
	return RenderContext::Instance()->GetImpl()->IsSupportETC2();
}

extern "C"
bool gum_avaliable_memory(int need_texture_area)
{
	return RenderContext::Instance()->GetImpl()->CheckAvailableMemory(need_texture_area);
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
	CU_STR gbk_name = StringHelper::UTF8ToGBK(name);
	return simp::PkgIDMgr::Instance()->QueryPkgID(gbk_name);
}

extern "C"
void gum_load_audio_ids(const char* filepath)
{
	simp::AudioIDMgr::Instance()->LoadAudioIDs(filepath);
}

extern "C"
int gum_get_audio_num()
{
	return simp::AudioIDMgr::Instance()->GetAudioSize();
}

extern "C"
void gum_get_all_audio_names(const char* names[])
{
	simp::AudioIDMgr::Instance()->GetAllAudioNames(names);
}

extern "C"
void gum_audio_set_path(const char* name, const char* filepath)
{
	if (name && filepath) {
		simp::AudioIDMgr::Instance()->SetAudioPath(name, filepath);
	}
}

/************************************************************************/
/* table                                                                */
/************************************************************************/

//extern "C"
//void gum_load_languages(const char* index_path, const char* data_path)
//{
//	TextTableLoader::LoadFromDB(index_path, data_path);
//}

extern "C"
void gum_load_languages(const char* filepath)
{
	TextTableLoader::LoadFromBin(filepath);
}

extern "C"
void gum_set_language(const char* lang)
{
	int lang_idx = s2::TextTable::Instance()->QueryType(lang);
	Config::Instance()->SetLanguage(lang_idx);
}

/************************************************************************/
/* pkg                                                                  */
/************************************************************************/

extern "C"
bool gum_pkg_exists(const char* name)
{
	CU_STR gbk_name = StringHelper::UTF8ToGBK(name);
	return simp::NodeFactory::Instance()->QueryPkg(gbk_name) != nullptr;
}

extern "C"
bool gum_create_pkg(const char* name, int id, const char* spr_path, const char* tex_path)
{
	CU_STR gbk_name = StringHelper::UTF8ToGBK(name);
	CU_STR gbk_spr_path = StringHelper::UTF8ToGBK(spr_path);
	CU_STR gbk_tex_path = StringHelper::UTF8ToGBK(tex_path);

	simp::PackagePtr s_pkg(CU_MAKE_UNIQUE<simp::Package>(gbk_spr_path, id));
	bool success = simp::NodeFactory::Instance()->AddPkg(s_pkg, gbk_name, id);
	if (!success) {
		return success;
	}

	timp::PackagePtr t_pkg(CU_MAKE_UNIQUE<timp::Package>(gbk_tex_path));
	success = timp::PkgMgr::Instance()->Add(t_pkg, id);
	assert(success);

	DTex::Instance()->CreatePkg(id);

	return true;
}

extern "C"
bool gum_create_pkg2(const char* name, int id, const char* pkg_path)
{
	CU_STR gbk_name = StringHelper::UTF8ToGBK(name);
	CU_STR gbk_pkg_path = StringHelper::UTF8ToGBK(pkg_path);

	struct fs_file* file = fs_open(gbk_pkg_path.c_str(), "rb");
	if (file == nullptr) {
		fault("Can't open pkg file: %s\n", gbk_pkg_path.c_str());
	}

	int epe_off = PkgFileParser::GetEpeIdxOffset(file);		
	simp::PackagePtr s_pkg(CU_MAKE_UNIQUE<simp::Package>(file, epe_off, id));
 	bool success = simp::NodeFactory::Instance()->AddPkg(s_pkg, gbk_name, id);
 	if (!success) {
 		return success;
 	}

	int ept_off = PkgFileParser::GetEptIdxOffset(file);	
	timp::PackagePtr t_pkg(CU_MAKE_UNIQUE<timp::Package>(file, ept_off));
	success = timp::PkgMgr::Instance()->Add(t_pkg, id);
	assert(success);

	DTex::Instance()->CreatePkg(id);

	// set epe path
	PkgFileParser::SetEPPath(file, gbk_pkg_path, id);

	fs_close(file);

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
	CU_STR gbk_name = StringHelper::UTF8ToGBK(name);
	const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(gbk_name);
	if (!pkg) {
		return -1;
	}

	return pkg->GetPageCount();
}

extern "C"
void gum_pkg_set_page_filepath(const char* name, int page, const char* filepath)
{
	CU_STR gbk_name = StringHelper::UTF8ToGBK(name);
	CU_STR gbk_filepath = StringHelper::UTF8ToGBK(filepath);
	const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(gbk_name);
	if (!pkg) {
		return;
	}

	const_cast<simp::Package*>(pkg)->SetPagePath(page, bimp::FilePath(gbk_filepath));
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
	CU_STR gbk_filepath = StringHelper::UTF8ToGBK(filepath);

	const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	if (!pkg) {
		return;
	}

	const_cast<timp::Package*>(pkg)->SetTexPath(tex, lod, bimp::FilePath(gbk_filepath));
}

extern "C"
char** gum_pkg_get_export_names(const char* name, int* count)
{
	CU_STR gbk_name = StringHelper::UTF8ToGBK(name);
	const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(gbk_name);
	if (!pkg) {
		*count = 0;
		return nullptr;
	}

	CU_VEC<CU_STR> names;
	pkg->GetExportNames(names);
	int sz = names.size();
	char** ret = (char**)malloc(sizeof(char*) * sz);
	for (int i = 0; i < sz; ++i) 
	{
		const CU_STR& src = names[i];
		char* dst = (char*)malloc(src.size() + 1);
		strcpy(dst, src.c_str());
		ret[i] = dst;
	}
	*count = sz;
	return ret;
}

/************************************************************************/
/* symbol                                                               */
/************************************************************************/

extern "C"
void* gum_create_sym_model(const void* model)
{
#ifndef S2_DISABLE_MODEL
	const m3::Model* m3_model = static_cast<const m3::Model*>(model);
	s2::ModelSymbol* sym = new s2::ModelSymbol();
	sym->SetModel(const_cast<m3::Model*>(m3_model));
	return sym;	
#else
	return nullptr;
#endif // S2_DISABLE_MODEL
}

//extern "C"
//void* gum_fetch_sym(uint32_t id)
//{
//	auto sym = SymbolPool::Instance()->Fetch(id);
//	return static_cast<void*>(sym.get();
//}

/************************************************************************/
/* sprite                                                               */
/************************************************************************/

extern "C"
void* gum_create_actor(const char* pkg, const char* spr)
{
	CU_STR gbk_pkg = StringHelper::UTF8ToGBK(pkg);
	CU_STR gbk_spr = StringHelper::UTF8ToGBK(spr);
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(gbk_pkg, gbk_spr);
	if (id == 0xffffffff) {
		return nullptr;
	} else {
		return gum_create_actor_by_id(id);
	}
}

extern "C"
void* gum_create_actor_by_id(int id)
{
	s2::ActorProxy* ret = nullptr;

	try {
		auto spr = SpriteFactory::Instance()->CreateFromSym(id, true);
		auto actor = s2::ActorFactory::Create(nullptr, spr);
		s2::ActorProxyPool::Instance()->Create(actor, ret);
	} catch (std::exception& e) {
		LOGW("create actor fail %d\n", id);
	}

	return ret;
}

extern "C"
void* gum_create_actor_from_file(const char* filepath)
{
	CU_STR gbk_filepath = StringHelper::UTF8ToGBK(filepath);
	auto spr = SpriteFactory::Instance()->Create(gbk_filepath);

	auto actor = s2::ActorFactory::Create(nullptr, spr);

	s2::ActorProxy* ret;
	s2::ActorProxyPool::Instance()->Create(actor, ret);
	return ret;
}

extern "C"
void* gum_fetch_actor_cached(const char* pkg, const char* spr, bool* is_new)
{
	CU_STR gbk_pkg = StringHelper::UTF8ToGBK(pkg);
	CU_STR gbk_spr = StringHelper::UTF8ToGBK(spr);
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(gbk_pkg, gbk_spr);
	if (id == 0xffffffff) {
		return nullptr;
	} else {
		auto actor = gum::ActorPool::Instance()->Fetch(id, *is_new);
		s2::ActorProxy* proxy;
		s2::ActorProxyPool::Instance()->Create(actor, proxy);
		return proxy;
	}
}

extern "C"
void gum_return_actor_cached(void* actor)
{
	delete static_cast<s2::ActorPtr*>(actor);
}

extern "C"
void gum_draw_text(const char* str, int x, int y, int w) 
{
	CU_STR gbk_str = StringHelper::UTF8ToGBK(str);

	S2_MAT mat;
	mat.Translate(static_cast<float>(x), static_cast<float>(y));
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
	sm::rect normal;
	normal.xmin = normal.ymin = 0;
	normal.xmax = normal.ymax = 1;
	if (src) 
	{
		sm::rect s;
		s.xmin = src->xmin;
		s.xmax = src->xmax;
		s.ymin = src->ymin;
		s.ymax = src->ymax;
		gum_rt->Draw(s, normal);
	} 
	else 
	{
		gum_rt->Draw(normal, normal);
	}
}

extern "C"
int gum_rt_get_texid(void* rt)
{
	return static_cast<RenderTarget*>(rt)->GetTexID();
}

extern "C"
void gum_rt_set_screen(void* rt)
{
	int texid = -1;
	if (rt) {
		texid = static_cast<RenderTarget*>(rt)->GetTexID();
	}
	s2::Blackboard::Instance()->SetScreenCacheTexID(texid);
}

/************************************************************************/
/* dtex                                                                 */
/************************************************************************/

extern "C"
void gum_dtex_c2_enable(bool enable)
{
	DTex::Instance()->SetC2Enable(enable);	
}

extern "C"
void gum_dtex_set_c2_max_edge(int max_edge)
{
	DTexC2Strategy::Instance()->SetMaxC2Edge(max_edge);
}

extern "C"
bool gum_dtex_cache_pkg_static_load(void* cache, int pkg_id, int lod, bool ref)
{
	if (!ref) {
		return dtex_cache_pkg_static_load(cache, pkg_id, lod) == 0;
	}

	CU_SET<int> pkgs;
	std::queue<int> buf;
	buf.push(pkg_id);
	while (!buf.empty())
	{
		int id = buf.front(); buf.pop();
		pkgs.insert(id);
		const simp::Package* pkg = simp::NodeFactory::Instance()->QueryPkg(id);
		if (!pkg) {
			continue;
		}
		auto& ref_pkgs = pkg->GetRefPkgs();
		for (int i = 0, n = ref_pkgs.size(); i < n; ++i) {
			buf.push(ref_pkgs[i]);
		}
	}

	CU_SET<int>::iterator itr = pkgs.begin();
	for ( ; itr != pkgs.end(); ++itr) {
		if (dtex_cache_pkg_static_load(cache, *itr, lod) != 0) {
			return false;
		}
	}
	return true;
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
	CU_STR std_str = StringHelper::FromChar(str);
	GTxt::Instance()->Draw(nullptr, s, mt, s2::Color(255, 255, 255, 255), s2::Color(0, 0, 0, 0), std_str.c_str(), 0, false);
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

/************************************************************************/
/* stat                                                                 */
/************************************************************************/

extern "C"
void gum_stat_enable_graph(bool enable)
{
	Statistics::Instance()->EnableGraph(enable);
}

extern "C"
void gum_stat_enable_console(bool enable)
{
	Statistics::Instance()->EnableConsole(enable);
}

extern "C"
void gum_stat_enable_file(bool enable)
{
	Statistics::Instance()->EnableFile(enable);
}

extern "C"
bool gum_stat_is_graph_enable()
{
	return Statistics::Instance()->IsGraphEnable();
}

extern "C"
bool gum_stat_is_console_enable()
{
	return Statistics::Instance()->IsConsoleEnable();
}

extern "C"
bool gum_stat_is_file_enable()
{
	return Statistics::Instance()->IsFileEnable();
}

extern "C"
void gum_stat_enable_tag(bool enable)
{
	StatTag::Instance()->Enable(enable);
}

extern "C"
bool gum_stat_is_tag_enable()
{
	return StatTag::Instance()->IsEnable();
}

extern "C"
void gum_stat_print_tag(const char* type, const char* msg)
{
	StatTag::Instance()->Print(
		StringHelper::FromChar(type),
		StringHelper::FromChar(msg));
}

extern "C"
void gum_stat_no_stat_begin()
{
	Statistics::Instance()->NoStatBegin();
}

extern "C"
void gum_stat_no_stat_end()
{
	Statistics::Instance()->NoStatEnd();
}

extern "C"
void gum_stat_fps_begin()
{
	StatFPS::Instance()->Begin();
}

extern "C"
void gum_stat_fps_end()
{
	StatFPS::Instance()->End();
}

extern "C"
int gum_stat_get_fps()
{
	return static_cast<int>(StatFPS::Instance()->GetFPS());
}

extern "C"
void gum_stat_opt_enable(bool enable)
{
	Statistics::Instance()->OptEnable(enable);
}

extern "C"
void gum_stat_update()
{
	StatFPS::Instance()->Update();
	Statistics::Instance()->Update();
}

extern "C"
void gum_stat_print()
{
	Statistics::Instance()->Print();
}

extern "C"
void gum_stat_reset()
{
	Statistics::Instance()->Reset();
}

extern "C"
void gum_stat_flush()
{
	Statistics::Instance()->Flush();
}

extern "C"
void gum_stat_set_mem(float tot, float lua)
{
	Statistics::Instance()->SetMem(tot, lua);
}

extern "C"
void gum_stat_task_clear()
{
	mt::TaskStat::Instance()->Clear();
}

extern "C"
void gum_stat_task_trigger_flush()
{
	StatTasks::Instance()->TriggerFlush();
}

extern "C"
void gum_stat_task_try_flush()
{
	StatTasks::Instance()->TryFlush();
}

/************************************************************************/
/* record screen                                                        */
/************************************************************************/

extern "C"
void gum_record_screen_set_enable(bool enable)
{
	StatScreen::Instance()->Enable(enable);
}

extern "C"
bool gum_record_screen_is_enable()
{
	return StatScreen::Instance()->IsEnable();	
}

extern "C"
void gum_record_screen_print(void* rt)
{
	RenderTarget* gum_rt = static_cast<RenderTarget*>(rt);
	StatScreen::Instance()->Print(gum_rt);
}

extern "C"
void gum_record_screen_flush()
{
	StatScreen::Instance()->Flush();
}

extern "C"
void gum_record_screen_clear()
{
	StatScreen::Instance()->Clear();
}

}
