#ifndef _GUM_IMAGE_H_
#define _GUM_IMAGE_H_

#include "Resource.h"
#include "ResourceManager.h"
#include "ResPath.h"

#include <SM_Vector.h>

#include <stdint.h>

namespace s2 { class Texture; }

namespace gum
{

class Image;
class ImageLoader;
typedef ResourceManager<Image> ImageMgr;

class Image : public Resource
{
public:
	Image();
	virtual ~Image();

	virtual bool LoadFromFile(const ResPath& res_path, bool async);

	void AsyncLoad(int format, int width, int height);

	sm::ivec2 GetSize() const {
		return sm::ivec2(m_width, m_height);
	}

	uint32_t GetTexID() const { return m_id; }

	s2::Texture* GetS2Tex() const { return m_s2_tex; }
	
	bool IsLoadFinished() const;
	void SetLoadFinished(bool finished);

//private:
	void LoadFromLoader(const ImageLoader& loader);

protected:
	ResPath m_res_path;

	int m_width, m_height;
	int m_format;

	uint32_t m_id;

	s2::Texture* m_s2_tex;

}; // Image

}

#endif // _GUM_IMAGE_H_
