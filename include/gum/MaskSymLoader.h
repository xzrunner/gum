#ifndef _GUM_MASK_SYM_LOADER_H_
#define _GUM_MASK_SYM_LOADER_H_

#include <cu/uncopyable.h>

#include <string>
#include <memory>

namespace s2 { class MaskSymbol; }
namespace simp { class NodeMask; }

namespace gum
{

class MaskSymLoader : private cu::Uncopyable
{
public:
	MaskSymLoader(const std::shared_ptr<s2::MaskSymbol>& sym);

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeMask* node);

private:
	std::shared_ptr<s2::MaskSymbol> m_sym;

}; // MaskSymLoader

}

#endif // _GUM_MASK_SYM_LOADER_H_
