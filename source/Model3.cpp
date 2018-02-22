#include <sprite2/config.h>
#ifndef S2_DISABLE_MODEL

#include "gum/Model3.h"
#include "gum/Image.h"
#include "gum/ImagePool.h"

#include <stat/StatImages.h>
#include <node3/ResourceAPI.h>

namespace gum
{

CU_SINGLETON_DEFINITION(Model3);

static std::map<std::string, std::shared_ptr<Image>> CACHE;

static void* 
create_img(const std::string& filepath)
{
	auto itr = CACHE.find(filepath);
	if (itr != CACHE.end()) {
		return itr->second.get();
	} else {
		auto img = ImagePool::Instance()->Create(
			st::StatImages::UNKNOWN_IMG_ID, bimp::FilePath(filepath.c_str()));
		CACHE.insert(std::make_pair(filepath, img));
		return img.get();
	}
}

static void 
release_img(void* img)
{
	auto& filepath = static_cast<Image*>(img)->GetResPath().GetFilepath();
	CACHE.erase(filepath.c_str());

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
	n3::ResourceAPI::Callback cb;
	cb.create_img  = create_img;
	cb.release_img = release_img;
	cb.get_tex_id  = get_tex_id;
	n3::ResourceAPI::InitCallback(cb);
}

}

#endif // S2_DISABLE_MODEL