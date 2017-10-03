#include "ImagePool.h"
#include "Image.h"

namespace gum
{

CU_SINGLETON_DEFINITION(ImagePool)

ImagePool::ImagePool()
{
}

std::shared_ptr<Image> ImagePool::Create(int pkg_id, const ResPath& res_path, bool async)
{
	auto itr = m_map_images.find(res_path);
	if (itr != m_map_images.end())
	{
		std::shared_ptr<Image> img = itr->second.lock();
		if (img) {
			return img;
		} else {
			m_map_images.erase(itr);
		}
	}

	std::shared_ptr<Image> img = std::make_shared<Image>(pkg_id, res_path, async);
	m_map_images.insert(std::make_pair(res_path, img));
	return img;
}

std::shared_ptr<Image> ImagePool::Query(const ResPath& res_path)
{
	auto itr = m_map_images.find(res_path);
	if (itr != m_map_images.end()) {
		std::shared_ptr<Image> img = itr->second.lock();
		if (img) {
			return img;
		}
	}
	return nullptr;
}

bool ImagePool::Add(const ResPath& res_path, const std::shared_ptr<Image>& img)
{
	auto ret = m_map_images.insert(std::make_pair(res_path, img));
	return ret.second;
}

bool ImagePool::Delete(const ResPath& res_path)
{
	auto itr = m_map_images.find(res_path);
	if (itr == m_map_images.end()) {
		return false;
	} else {
		m_map_images.erase(itr);
		return true;
	}
}

void ImagePool::GC()
{
	auto itr = m_map_images.begin();
	for (; itr != m_map_images.end(); )
	{
		if (itr->second.expired()) {
			itr = m_map_images.erase(itr);
		} else {
			++itr;
		}
	}
}

}