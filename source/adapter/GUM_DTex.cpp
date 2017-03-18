#include "GUM_DTex.h"
#include "ImageLoader.h"
#include "RenderContext.h"
#include "ProxyImage.h"
#include "DTexC2Strategy.h"

#include <logger.h>
#include <bimp/BIMP_ImportStream.h>
#include <timp/TIMP_Package.h>
#include <timp/PkgMgr.h>
#include <timp/TextureLoader.h>
#include <simp/RelocateTexcoords.h>
#include <simp/PageVisitor.h>
#include <simp/RelocateTexcoords.h>
#include <simp/NodeFactory.h>
#include <dtex2/RenderAPI.h>
#include <dtex2/ResourceAPI.h>
#include <dtex2/DTEX_Package.h>
#include <dtex2/DTEX_PkgMgr.h>
#include <dtex2/TextureRaw.h>
#include <dtex2/Cache.h>
#include <dtex2/CacheMgr.h>
#include <dtex2/CacheAPI.h>
#include <dtex2/CacheSymbol.h>
#include <dtex2/CS_Node.h>
#include <dtex2/CacheGlyph.h>
#include <dtex2/AsyncTask.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/ShapeShader.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/FilterShader.h>
#include <sprite2/S2_RVG.h>
#include <sprite2/S2_RenderContext.h>
#include <sprite2/RenderCtxStack.h>
#include <sprite2/RenderScissor.h>
#include <sprite2/DrawNode.h>
#include <unirender/UR_RenderContext.h>

#include <string>

#include <assert.h>

namespace dtex { class Package; }

namespace gum
{

SINGLETON_DEFINITION(DTex);

static void (*DRAW_BEGIN)() = NULL;
static void (*DRAW_END)() = NULL;

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

static void 
clear_color_part(float xmin, float ymin, float xmax, float ymax)
{
	RenderContext::Instance()->GetImpl()->EnableBlend(false);
//	glBlendFunc(GL_ONE, GL_ZERO);

	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();

	mgr->SetShader(sl::SHAPE2);
	sl::ShapeShader* shader = static_cast<sl::ShapeShader*>(mgr->GetShader());
	
	shader->SetColor(0);
//	shader->SetColor(0xff0000ff);

	int w = RenderContext::Instance()->GetWidth(),
		h = RenderContext::Instance()->GetHeight();
	xmin = w * 0.5f * (xmin - 1);
	xmax = w * 0.5f * (xmax - 1);
	ymin = h * 0.5f * (ymin + 1);
	ymax = h * 0.5f * (ymax + 1);

	std::vector<sm::vec2> triangles(4);
	triangles[0].Set(xmin, ymin);
	triangles[1].Set(xmin, ymax);
	triangles[2].Set(xmax, ymin);
	triangles[3].Set(xmax, ymax);

	s2::RVG::TriangleStrip(triangles);

	shader->Commit();
// 	ShaderLab::Instance()->Flush();

//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	RenderContext::Instance()->GetImpl()->EnableBlend(true);

	//////////////////////////////////////////////////////////////////////////

//	dtex_gl_clear_color(0, 0, 0, 0);
}

static void 
set_program()
{
	// todo
	sl::ShaderMgr::Instance()->SetShader(sl::SPRITE2);
}

static void 
set_blend(int mode)
{
	// todo
	assert(0);

	assert(mode == 0);
//	ShaderMgr::Instance()->SetBlendMode(0);
}

static void 
draw_begin()
{
	if (DRAW_BEGIN) {
		DRAW_BEGIN();
	} else {
		s2::RenderCtxStack::Instance()->Push(s2::RenderContext(2, 2, 0, 0));
	}

	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	sl_mgr->SetShader(sl::SPRITE2);
	sl::Sprite2Shader* sl_shader = static_cast<sl::Sprite2Shader*>(sl_mgr->GetShader());
	sl_shader->SetColor(0xffffffff, 0);
	sl_shader->SetColorMap(0x000000ff, 0x0000ff00, 0x00ff0000);
}

static void 
draw(const float _vertices[8], const float _texcoords[8], int texid)
{
	sm::vec2 vertices[4], texcoords[4];
	for (int i = 0; i < 4; ++i) {
		vertices[i].x  = _vertices[i * 2];
		vertices[i].y  = _vertices[i * 2 + 1];
		texcoords[i].x = _texcoords[i * 2];
		texcoords[i].y = _texcoords[i * 2 + 1];
	}

	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	switch (sl_mgr->GetShaderType()) 
	{
	case sl::SPRITE2:
		{
			sl::Sprite2Shader* shader = static_cast<sl::Sprite2Shader*>(sl_mgr->GetShader());
			shader->Draw(&vertices[0].x, &texcoords[0].x, texid);
		}
		break;
	case sl::FILTER:
		{
			sl::FilterShader* shader = static_cast<sl::FilterShader*>(sl_mgr->GetShader());
			shader->Draw(&vertices[0].x, &texcoords[0].x, texid);
		}
		break;
	default:
		break;
	}
}

static void 
draw_end()
{
	sl::ShaderMgr::Instance()->FlushShader();

	if (DRAW_END) {
		DRAW_END();
	} else {
		s2::RenderCtxStack::Instance()->Pop();
	}
}

static void 
draw_flush()
{
	sl::Shader* shader = sl::ShaderMgr::Instance()->GetShader();
	if (shader) {
		shader->Commit();
	}
}

static void
scissor_push(int x, int y, int w, int h)
{
	s2::RenderScissor::Instance()->Push(x, y, w, h, false, true);
}

static void 
scissor_pop()
{
	s2::RenderScissor::Instance()->Pop(false);
}

static void 
scissor_close()
{
	s2::RenderScissor::Instance()->Close();
}

static void 
scissor_open()
{
	s2::RenderScissor::Instance()->Open();
}

/************************************************************************/
/* Resource                                                             */
/************************************************************************/

static const std::string& 
get_tex_filepath(int pkg_id, int tex_idx, int lod_layer) 
{
	const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	assert(pkg);
	return pkg->GetTexPath(tex_idx, lod_layer);
}

class FileLoader : public bimp::FileLoader
{
public:
	FileLoader(const std::string& filepath, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
		: bimp::FileLoader(filepath, use_cache)
		, m_parser_cb(parser_cb)
		, m_ud(ud)
	{}

protected:
	virtual void OnLoad(bimp::ImportStream& is) 
	{
		m_parser_cb(is.Stream(), is.Size(), m_ud);
	}

private:
	void (*m_parser_cb)(const void* data, size_t size, void* ud);
	void* m_ud;

}; // FileLoader

static void 
load_file(const std::string& filepath, bool async, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
{
	FileLoader loader(filepath, async, parser_cb, ud);
	loader.Load();
}

static void 
load_texture(int pkg_id, int tex_idx) 
{
	const timp::Package* t_pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	assert(t_pkg);
	const std::string& filepath = t_pkg->GetTexPath(tex_idx, 0);
	
	ImageLoader loader(filepath);
	bool ret = loader.Load();
	if (!ret) {
		return;
	}

	const dtex::Package* d_pkg = dtex::PkgMgr::Instance()->Query(pkg_id);
	dtex::Texture* tex = d_pkg->GetTexture(tex_idx);
	assert(tex->Type() == dtex::TEX_RAW);
	dtex::TextureRaw* raw_tex = static_cast<dtex::TextureRaw*>(tex);
	raw_tex->SetID(loader.GetID());
	raw_tex->SetSize(loader.GetWidth(), loader.GetHeight());
	raw_tex->SetFormat(loader.GetFormat());
}

static void 
load_texture_cb(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud) 
{
	const timp::Package* t_pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	assert(t_pkg);
	const std::string& filepath = t_pkg->GetTexPath(tex_idx, 0);

	timp::TextureLoader loader(filepath);
	loader.Load();

	cb(loader.GetFormat(), loader.GetWidth(), loader.GetHeight(), loader.GetData(), ud);
}

static void 
load_texture_cb2(const void* data, size_t size, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud) 
{
	timp::TextureLoader loader(static_cast<const char*>(data), size);
	loader.Load();

	cb(loader.GetFormat(), loader.GetWidth(), loader.GetHeight(), loader.GetData(), ud);
}

/************************************************************************/
/* Cache                                                                */
/************************************************************************/

static void
relocate_pkg(int src_pkg, int src_tex, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax)
{
	simp::RelocateTexcoords::Item item;
	item.src_pkg = src_pkg;
	item.src_tex = src_tex;
	item.dst_tex_id = dst_tex_id;
	item.dst_w = dst_w;
	item.dst_h = dst_h;
	item.dst_xmin = dst_xmin;
	item.dst_ymin = dst_ymin;
	item.dst_xmax = dst_xmax;
	item.dst_ymax = dst_ymax;
	simp::RelocateTexcoords::Instance()->Add(item);

	std::string filepath = ProxyImage::GetFilepath(dst_tex_id);
	Image* img = ImageMgr::Instance()->Query(filepath);
	if (img) {
		ProxyImage* p_img = static_cast<ProxyImage*>(img);
		p_img->Init(dst_tex_id, dst_w, dst_h, dst_fmt);
	} else {
		img = new ProxyImage(dst_tex_id, dst_w, dst_h, dst_fmt);
		std::string filepath = ProxyImage::GetFilepath(dst_tex_id);
		bool succ = ImageMgr::Instance()->Add(filepath, img);
		assert(succ);
		img->RemoveReference();
	}
}

class RelocatePageVisitor : public simp::PageVisitor
{
public:
	virtual void Visit(const simp::Page* page) 
	{
		simp::RelocateTexcoords::Instance()->Do(page);
	}

}; // RelocatePageVisitor

static void
relocate_pkg_finish()
{
	RelocatePageVisitor visitor;
	simp::NodeFactory::Instance()->Traverse(visitor);
}

/************************************************************************/
/* Glyph                                                                */
/************************************************************************/

static void 
glyph_load_start()
{
	DTex::Instance()->LoadSymStart();
}

static void 
glyph_load(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key)
{
	DTex::Instance()->DrawGlyph(tex_id, tex_w, tex_h, r, key);
}

static void 
glyph_load_finish()
{
	DTex::Instance()->LoadSymFinish();
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

void DTex::InitHook(void (*draw_begin)(), void (*draw_end)())
{
 	DRAW_BEGIN = draw_begin;
 	DRAW_END = draw_end;
}

DTex::DTex()
{
	dtex::RenderAPI::Callback render_cb;
	render_cb.clear_color_part = clear_color_part;
	render_cb.set_program      = set_program;
	render_cb.set_blend        = set_blend;
	render_cb.draw_begin       = draw_begin;
	render_cb.draw             = draw;
	render_cb.draw_end         = draw_end;
	render_cb.draw_flush       = draw_flush;
	render_cb.scissor_push     = scissor_push;
	render_cb.scissor_pop      = scissor_pop;
	render_cb.scissor_close    = scissor_close;
	render_cb.scissor_open     = scissor_open;

	dtex::RenderAPI::InitCallback(render_cb);
	dtex::RenderAPI::InitRenderContext(RenderContext::Instance()->GetImpl());

	dtex::ResourceAPI::Callback res_cb;
	res_cb.get_tex_filepath = get_tex_filepath;
	res_cb.load_file        = load_file;
	res_cb.load_texture     = load_texture;
	res_cb.load_texture_cb  = load_texture_cb;
	res_cb.load_texture_cb2 = load_texture_cb2;
	dtex::ResourceAPI::InitCallback(res_cb);

	dtex::CacheAPI::Callback cache_cb;
	cache_cb.relocate_pkg        = relocate_pkg;
	cache_cb.relocate_pkg_finish = relocate_pkg_finish;
	dtex::CacheAPI::InitCallback(cache_cb);

	m_c2 = new dtex::CacheSymbol(2048, 2048);
	dtex::CacheMgr::Instance()->Add(m_c2, "C2");

	dtex::CacheGlyph::Callback glyph_cb;
	glyph_cb.load_start  = glyph_load_start;
	glyph_cb.load        = glyph_load;
	glyph_cb.load_finish = glyph_load_finish;
	m_cg = new dtex::CacheGlyph(256, 512, glyph_cb);
	dtex::CacheMgr::Instance()->Add(m_cg, "CG");
}

void DTex::CreatePkg(int pkg_id)
{
	const timp::Package* src = timp::PkgMgr::Instance()->Query(pkg_id);
	assert(src);

	dtex::Package* dst = new dtex::Package(pkg_id);

	const std::vector<timp::Package::TextureDesc>& textures 
		= src->GetAllTextures();
	for (int i = 0, n = textures.size(); i < n; ++i) 
	{
		const timp::Package::TextureDesc& desc = textures[i];
		dtex::Texture* tex = new dtex::TextureRaw(1);
		tex->SetFormat(desc.type);
		tex->SetSize(desc.w, desc.h);
		dst->AddTexture(tex);
	}

	dtex::PkgMgr::Instance()->Add(dst, pkg_id);
}

void DTex::LoadSymStart()
{
	m_c2->LoadStart();
}

void DTex::LoadSymbol(UID sym_id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region,
					  int padding, int extrude, int src_extrude)
{
	dtex::Rect r;
	r.xmin = region.xmin;
	r.ymin = region.ymin;
	r.xmax = region.xmax;
	r.ymax = region.ymax;
	m_c2->Load(tex_id, tex_w, tex_h, r, sym_id, padding, extrude, src_extrude);
}

void DTex::LoadSymFinish()
{
	m_c2->LoadFinish();
}

const float* DTex::QuerySymbol(UID sym_id, int* tex_id) const
{
	const dtex::CS_Node* node = m_c2->Query(sym_id);
	if (node) {
		*tex_id = m_c2->GetTexID();
		return node->GetTexcoords();
	} else {
		return NULL;
	}
}

void DTex::DrawGlyph(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key)
{
	m_c2->Load(tex_id, tex_w, tex_h, r, key, 1, 0);
}

void DTex::LoadGlyph(uint32_t* bitmap, int width, int height, uint64_t key)
{
	m_cg->Load(bitmap, width, height, key);
}

void DTex::Clear()
{
}

void DTex::Flush()
{
	dtex::AsyncTask::Instance()->Update();
	m_cg->Flush();
}

void DTex::DebugDraw() const
{
// 	const std::map<std::string, dtex::Cache*>& caches 
// 		= dtex::CacheMgr::Instance()->FetchAll();
// 	std::map<std::string, dtex::Cache*>::const_iterator itr = caches.begin();
// 	for ( ; itr != caches.end(); ++itr) {
// 		itr->second->DebugDraw();
// 	}

	//////////////////////////////////////////////////////////////////////////

	m_c2->DebugDraw();
}

}