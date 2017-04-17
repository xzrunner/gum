#include "ImageSymLoader.h"
#include "GUM_ImageSymbol.h"
#include "Image.h"

namespace gum
{

ImageSymLoader::ImageSymLoader(ImageSymbol* sym)
	: m_sym(sym)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

ImageSymLoader::~ImageSymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

void ImageSymLoader::Load(const std::string& filepath, bool async)
{
	Image* img = ImageMgr::Instance()->Create(filepath, async);
	if (img) {
		m_sym->SetImage(img);
		img->RemoveReference();
	}
}

}