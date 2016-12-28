#ifndef _GUM_PROXY_IMAGE_H_
#define _GUM_PROXY_IMAGE_H_

#include "Image.h"

namespace gum
{

class ProxyImage : public Image
{
public:
	ProxyImage(int id, int w, int h, int format);
	virtual ~ProxyImage();

	void Init(int id, int w, int h, int format);

	static std::string GetFilepath(int id);

}; // ProxyImage

}

#endif // _GUM_PROXY_IMAGE_H_