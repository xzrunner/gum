#include "GUM_DTex2.h"
#include "ImageLoader.h"
#include "RenderContext.h"
#include "ProxyImage.h"

#include <logger.h>
#include <timp/Package.h>
#include <timp/PkgMgr.h>
#include <timp/TextureLoader.h>
#include <simp/RelocateTexcoords.h>
#include <dtex2/RenderAPI.h>
#include <dtex2/ResourceAPI.h>
#include <dtex2/Package.h>
#include <dtex2/PkgMgr.h>
#include <dtex2/TextureRaw.h>
#include <dtex2/Cache.h>
#include <dtex2/CacheMgr.h>
#include <dtex2/CacheAPI.h>
#include <shaderlab.h>
#include <sprite2/S2_RVG.h>
#include <sprite2/RenderCtx.h>
#include <sprite2/RenderCtxStack.h>
#include <sprite2/RenderScissor.h>

#include <string>

#include <assert.h>

namespace dtex { class Package; }

namespace gum
{

SINGLETON_DEFINITION(DTex2);

static void (*DRAW_BEGIN)() = NULL;
static void (*DRAW_END)() = NULL;

/************************************************************************/
/* Texture                                                              */
/************************************************************************/

static int 
texture_create(const void* data, int width, int height, int format)
{
	return RenderContext::Instance()->CreateTexture(static_cast<const uint8_t*>(data), width, height, format);
}

static void 
texture_release(int id)
{
	RenderContext::Instance()->ReleaseTexture(id);
}

static void 
texture_update(const void* pixels, int w, int h, unsigned int id)
{
	RenderContext::Instance()->UpdateTexture(static_cast<const uint8_t*>(pixels), w, h, id);
}

static void 
sub_tex_update(const void* pixels, int x, int y, int w, int h, unsigned int id)
{
	RenderContext::Instance()->UpdateSubTex(static_cast<const uint8_t*>(pixels), x, y, w, h, id);
}

/************************************************************************/
/* Target                                                               */
/************************************************************************/

static int 
create_target(int id) 
{
	GLuint gl_id = id;
	glGenFramebuffers(1, &gl_id);
	return gl_id;
}

static void 
release_target(int id) 
{
	GLuint gl_id = id;
	glDeleteFramebuffers(1, &gl_id);
}

static void 
target_bind_texture(int tex_id) 
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0);
}

static void 
target_bind(int id) 
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

static int 
check_target_status() 
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		return 1;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		LOGW("%s", "Framebuffer incomplete: Attachment is NOT complete.\n");
		return 0;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		LOGW("%s", "Framebuffer incomplete: No image is attached to FBO.\n");
		return 0;
#if !defined(_WIN32) && !defined(__MACOSX)
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		LOGW("%s", "Framebuffer incomplete: Attached images have different dimensions.\n");
		return 0;
#endif
	case GL_FRAMEBUFFER_UNSUPPORTED:
		LOGW("%s", "Unsupported by FBO implementation.\n");
		return 0;
	default:
		LOGW("%s", "Unknow error.\n");
		return 0;
	}
}

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

static void 
clear_color(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void 
clear_color_part(float xmin, float ymin, float xmax, float ymax)
{
	render_gl_blend_disable();
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
	render_gl_blend_enable();

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
set_texture(int id)
{
	sl::ShaderMgr::Instance()->GetContext()->SetTexture(id, 0);
}

static int 
get_texture()
{
	return sl::ShaderMgr::Instance()->GetContext()->GetTexture();
}

static void 
set_target(int id)
{
	sl::ShaderMgr::Instance()->GetContext()->SetTarget(id);
}

static int 
get_target()
{
	return sl::ShaderMgr::Instance()->GetContext()->GetTarget();
}

static void 
draw_begin()
{
	if (DRAW_BEGIN) {
		DRAW_BEGIN();
	} else {
		s2::RenderCtxStack::Instance()->Push(s2::RenderCtx(2, 2, 0, 0), false);
	}

	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	sl_mgr->SetShader(sl::SPRITE2);
	sl::Sprite2Shader* sl_shader = static_cast<sl::Sprite2Shader*>(sl_mgr->GetShader());
	sl_shader->SetColor(0xffffffff, 0);
	sl_shader->SetColorMap(0x000000ff, 0x0000ff00, 0x00ff0000);
}

static void 
draw(const float vb[16], int texid)
{
	sm::vec2 vertices[4], texcoords[4];
	for (int i = 0; i < 4; ++i) {
		vertices[i].x  = vb[i * 4];
		vertices[i].y  = vb[i * 4 + 1];
		texcoords[i].x = vb[i * 4 + 2];
		texcoords[i].y = vb[i * 4 + 3];
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
	}
}

static void 
draw_end()
{
	sl::ShaderMgr::Instance()->GetShader()->Commit();

	if (DRAW_END) {
		DRAW_END();
	} else {
		s2::RenderCtxStack::Instance()->Pop(false);
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
scissor_enable(bool enable)
{
	if (enable) {
		s2::RenderScissor::Instance()->Open();
	} else {
		s2::RenderScissor::Instance()->Close();
	}
}

static void 
scissor(int x, int y, int w, int h)
{
	render_setscissor(x, y, w, h);
}

static void 
viewport_push(int x, int y, int w, int h)
{
	sl::RenderContext* ctx = sl::ShaderMgr::Instance()->GetContext();
	ctx->ViewportPush(x, y, w, h);
}

static void 
viewport_pop()
{
	sl::RenderContext* ctx = sl::ShaderMgr::Instance()->GetContext();
	ctx->ViewportPop();
}

/************************************************************************/
/* util                                                                 */
/************************************************************************/

static bool 
out_of_memory()
{
	GLenum err = glGetError();
	return err == GL_OUT_OF_MEMORY;
}

static bool 
is_texture(unsigned int id)
{
	return glIsTexture(id);
}

static void 
check_error()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		exit(1);
	}
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

//////////////////////////////////////////////////////////////////////////

DTex2::DTex2()
{
	dtex::RenderAPI::TextureCB texture_cb;
	texture_cb.texture_create  = texture_create;
	texture_cb.texture_release = texture_release;
	texture_cb.texture_update  = texture_update;
	texture_cb.sub_tex_update  = sub_tex_update;

	dtex::RenderAPI::TargetCB target_cb;
	target_cb.create_target       = create_target;
	target_cb.release_target      = release_target;
	target_cb.target_bind_texture = target_bind_texture;
	target_cb.target_bind         = target_bind;
	target_cb.check_target_status = check_target_status;

	dtex::RenderAPI::DrawCB draw_cb;
	draw_cb.clear_color      = clear_color;
	draw_cb.clear_color_part = clear_color_part;
	draw_cb.set_program      = set_program;
	draw_cb.set_blend        = set_blend;
	draw_cb.set_texture      = set_texture;
	draw_cb.get_texture      = get_texture;
	draw_cb.set_target       = set_target;
	draw_cb.get_target       = get_target;
	draw_cb.draw_begin       = draw_begin;
	draw_cb.draw             = draw;
	draw_cb.draw_end         = draw_end;
	draw_cb.draw_flush       = draw_flush;
	draw_cb.scissor_enable   = scissor_enable;
	draw_cb.scissor          = scissor;
	draw_cb.viewport_push    = viewport_push;
	draw_cb.viewport_pop     = viewport_pop;

	dtex::RenderAPI::UtilCB util_cb;
	util_cb.out_of_memory = out_of_memory;
	util_cb.is_texture    = is_texture;
	util_cb.check_error   = check_error;

	dtex::RenderAPI::InitTexCB(texture_cb, target_cb, draw_cb, util_cb);

	dtex::ResourceAPI::Callback res_cb;
	res_cb.load_texture    = load_texture;
	res_cb.load_texture_cb = load_texture_cb;
	dtex::ResourceAPI::InitCallback(res_cb);

	dtex::CacheAPI::Callback cache_cb;
	cache_cb.relocate_pkg = relocate_pkg;
	dtex::CacheAPI::InitCallback(cache_cb);
}

void DTex2::CreatePkg(int pkg_id)
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

void DTex2::DebugDraw() const
{
	const std::map<std::string, dtex::Cache*>& caches 
		= dtex::CacheMgr::Instance()->FetchAll();
	std::map<std::string, dtex::Cache*>::const_iterator itr = caches.begin();
	for ( ; itr != caches.end(); ++itr) {
		itr->second->DebugDraw();
	}
}

}