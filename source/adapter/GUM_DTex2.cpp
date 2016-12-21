#include "GUM_DTex2.h"
#include "opengl.h"

#include <logger.h>
#include <timp/Package.h>
#include <timp/PkgMgr.h>
#include <dtex2/Render.h>
#include <dtex2/Resource.h>
#include <dtex2/Package.h>
#include <dtex2/PkgMgr.h>
#include <dtex2/TextureRaw.h>

#include <string>

#include <assert.h>

namespace dtex { class Package; }

namespace gum
{

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
		dtex::Texture* tex = new dtex::TextureRaw(1, desc.type);
		tex->SetSize(desc.w, desc.h);
		dst->AddTexture(tex);
	}

	dtex::PkgMgr::Instance()->Add(dst, pkg_id);
}

/************************************************************************/
/* Texture                                                              */
/************************************************************************/

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
/* Resource                                                             */
/************************************************************************/

static const std::string& 
get_tex_filepath(int pkg_id, int tex_idx, int lod_layer) 
{
	const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
	assert(pkg);
	return pkg->GetTexPath(tex_idx, lod_layer);
}

DTex2::DTex2()
{
	dtex::Render::TargetCB target_cb;
	target_cb.create_target       = create_target;
	target_cb.release_target      = release_target;
	target_cb.target_bind_texture = target_bind_texture;
	target_cb.target_bind         = target_bind;
	target_cb.check_target_status = check_target_status;

//	dtex::Render::InitTexCB();

	dtex::Resource::Callback res_cb;
	res_cb.get_tex_filepath = get_tex_filepath;
	dtex::Resource::InitCallback(res_cb);
}

}