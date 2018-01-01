#include <sprite2/config.h>
#ifndef S2_DISABLE_MODEL

#include "gum/Model3.h"
#include "gum/Image.h"
#include "gum/ImagePool.h"

#include <sprite2/StatImages.h>
#include <model3/ResourceAPI.h>

namespace gum
{

CU_SINGLETON_DEFINITION(Model3);

static void* 
create_img(const std::string& filepath)
{
	auto img = ImagePool::Instance()->Create(
		s2::StatImages::UNKNOWN_IMG_ID, bimp::FilePath(filepath.c_str()));
	return img.get();
}

static void 
release_img(void* img)
{
	ImagePool::Instance()->Delete(
		static_cast<Image*>(img)->GetResPath());
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