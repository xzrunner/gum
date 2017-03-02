#include "GUM_TexturePolygon.h"
#include "SymbolPool.h"

#include <sprite2/ImageSymbol.h>
#include <sprite2/SymType.h>

#include <assert.h>

namespace gum
{

TexturePolygon::TexturePolygon(const std::string& filepath)
{
	s2::Symbol* sym = SymbolPool::Instance()->Fetch(filepath);
	assert(sym->Type() == s2::SYM_IMAGE);
	m_img = dynamic_cast<s2::ImageSymbol*>(sym);
}

}