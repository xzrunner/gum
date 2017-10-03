#ifndef _GUM_IMAGE_POOL_H_
#define _GUM_IMAGE_POOL_H_

#include "ResPath.h"

#include <cu/cu_macro.h>

#include <map>
#include <memory>

namespace gum
{

class Image;

class ImagePool
{
public:
	std::shared_ptr<Image> Create(int pkg_id, const ResPath& res_path, bool async = false);

	std::shared_ptr<Image> Query(const ResPath& res_path);
	bool Add(const ResPath& res_path, const std::shared_ptr<Image>& img);
	bool Delete(const ResPath& res_path);

	void GC();
	void Clear() { m_map_images.clear(); }

	int Count() const { return m_map_images.size(); }

private:
	std::map<ResPath, std::weak_ptr<Image>> m_map_images;

	CU_SINGLETON_DECLARATION(ImagePool)

}; // ImagePool

}

#endif // _GUM_IMAGE_POOL_H_