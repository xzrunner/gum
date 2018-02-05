#ifndef _GUM_IMAGE_H_
#define _GUM_IMAGE_H_

#include <bimp/FilePath.h>

#include <SM_Vector.h>

#include <memory>

#include <stdint.h>

namespace s2loader { class ImageLoader; }

namespace gum
{

class Image;
class Texture;

class Image : public std::enable_shared_from_this<Image>
{
public:
	Image();
	Image(int pkg_id, const bimp::FilePath& res_path, bool async);
	virtual ~Image();

	void AsyncLoad(int pkg_id, int format, int width, int height);

	sm::ivec2 GetSize() const;

	uint32_t GetTexID() const;

	const std::shared_ptr<Texture>& GetTexture() const { return m_texture; }
	
	bool IsLoadFinished() const;
	void SetLoadFinished(bool finished);

	const bimp::FilePath& GetResPath() const { return m_res_path; }

//private:
	void LoadFromLoader(const s2loader::ImageLoader& loader);

	static int GetAllImgCount();

protected:
	int m_pkg_id;

	bimp::FilePath m_res_path;

	std::shared_ptr<Texture> m_texture;

}; // Image

}

#endif // _GUM_IMAGE_H_
