#include <sprite2/s2_config.h>
#ifndef S2_DISABLE_MODEL

#include "GUM_Model3.h"
#include "Image.h"

//#include <model3/ResourceAPI.h>

#include <string>

namespace gum
{

CU_SINGLETON_DEFINITION(Model3);

static void* 
create_img(const std::string& filepath)
{
//	return ImageMgr::Instance()->Create(filepath);
	return nullptr;
}

static void 
release_img(void* img)
{
	Image* gum_img = static_cast<Image*>(img);
	gum_img->RemoveReference();
}

static int 
get_tex_id(const void* img)
{
	if (!img) {
		return -1;
	} else {
		return static_cast<const Image*>(img)->GetTexID();
	}
}

Model3::Model3()
{
	m3::ResourceAPI::Callback cb;
	cb.create_img  = create_img;
	cb.release_img = release_img;
	cb.get_tex_id  = get_tex_id;
	m3::ResourceAPI::InitCallback(cb);
}

}

#endif // S2_DISABLE_MODEL