#ifndef _GUM_TEXTURE_SYM_LOADER_H_
#define _GUM_TEXTURE_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <json/json.h>

#include <memory>

namespace s2 { class TextureSymbol; }
namespace simp { class NodeTexture; }

namespace gum
{

class TextureSymLoader : private cu::Uncopyable
{
public:
	TextureSymLoader(const std::shared_ptr<s2::TextureSymbol>& sym);

	void LoadJson(const CU_STR& filepath);	
	void LoadBin(const simp::NodeTexture* node);

private:
	std::shared_ptr<s2::TextureSymbol> m_sym;

}; // TextureSymLoader

}

#endif // _GUM_TEXTURE_SYM_LOADER_H_