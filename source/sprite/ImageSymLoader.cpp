#include "ImageSymLoader.h"
#include "GUM_ImageSymbol.h"
#include "Image.h"

#include <simp/NodeID.h>

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

void ImageSymLoader::Load(const ResPath& res_path, float scale, bool async)
{
	int pkg_id = simp::NodeID::GetPkgID(m_sym->GetID());
	Image* img = ImageMgr::Instance()->Create(pkg_id, res_path, async);
	if (!img) {
		return;
	}
	m_sym->SetImage(img);
	sm::ivec2 sz = img->GetSize();
	m_sym->SetRegion(sm::ivec2(0, 0), sz, sm::vec2(0, 0), 0, scale);
	img->RemoveReference();
}

}