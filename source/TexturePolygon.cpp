#include "gum/TexturePolygon.h"
#include "gum/SymbolPool.h"

#include <sprite2/ImageSymbol.h>
#include <sprite2/SymType.h>

#include <assert.h>

namespace gum
{

TexturePolygon::TexturePolygon(const CU_STR& filepath)
{
	auto sym = SymbolPool::Instance()->Fetch(filepath);
	if (sym) {
		assert(sym->Type() == s2::SYM_IMAGE);
		m_img = S2_VI_PTR_DOWN_CAST<s2::ImageSymbol>(sym);
	}
}

}