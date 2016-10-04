#include "TexturePolygon.h"
#include "SymbolFactory.h"

#include <sprite2/ImageSymbol.h>
#include <sprite2/SymType.h>

#include <assert.h>

namespace gum
{

TexturePolygon::TexturePolygon(const std::string& filepath)
{
	s2::Symbol* sym = SymbolFactory::Instance()->Create(filepath);
	assert(sym->Type() == s2::SYM_IMAGE);
	m_img = dynamic_cast<s2::ImageSymbol*>(sym);
}

}