#ifndef _GUM_MASK_SYM_LOADER_H_
#define _GUM_MASK_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

namespace s2 { class MaskSymbol; }
namespace simp { class NodeMask; }

namespace gum
{

class MaskSymLoader : private cu::Uncopyable
{
public:
	MaskSymLoader(s2::MaskSymbol& sym);

	void LoadJson(const CU_STR& filepath);
	void LoadBin(const simp::NodeMask* node);

private:
	s2::MaskSymbol& m_sym;

}; // MaskSymLoader

}

#endif // _GUM_MASK_SYM_LOADER_H_
