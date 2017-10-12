#ifndef _GUM_IMAGE_H_
#define _GUM_IMAGE_H_

#include <bimp/FilePath.h>

#include <SM_Vector.h>

#include <memory>

#include <stdint.h>

namespace s2 { class Texture; }

namespace gum
{

class Image;
class ImageLoader;

class Image : public std::enable_shared_from_this<Image>
{
public:
	Image();
	Image(int pkg_id, const bimp::FilePath& res_path, bool async);
	virtual ~Image();

	void AsyncLoad(int pkg_id, int format, int width, int height);

	sm::ivec2 GetSize() const {
		return sm::ivec2(m_width, m_height);
	}

	uint32_t GetTexID() const { return m_id; }

	const std::shared_ptr<s2::Texture>& GetS2Tex() const { return m_s2_tex; }
	
	bool IsLoadFinished() const;
	void SetLoadFinished(bool finished);

	const bimp::FilePath& GetResPath() const { return m_res_path; }

//private:
	void LoadFromLoader(const ImageLoader& loader);

	static int GetAllImgCount();

protected:
	int m_pkg_id;

	bimp::FilePath m_res_path;

	int m_width, m_height;
	int m_format;

	uint32_t m_id;

	std::shared_ptr<s2::Texture> m_s2_tex;

}; // Image

}

#endif // _GUM_IMAGE_H_
