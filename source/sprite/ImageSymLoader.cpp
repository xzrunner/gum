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

void ImageSymLoader::Load(const std::string& filepath, float scale, bool async)
{
	Image* img = ImageMgr::Instance()->Create(filepath, async);
	if (!img) {
		return;
	}
	m_sym->SetImage(img);
	sm::ivec2 sz = img->GetSize();
	m_sym->SetRegion(sm::ivec2(0, 0), sz, sm::vec2(0, 0), 0, scale);
	img->RemoveReference();
}

}