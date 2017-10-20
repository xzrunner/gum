#ifndef _GUM_FAST_BLEND_MODES_H_
#define _GUM_FAST_BLEND_MODES_H_

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>
#include <sprite2/FastBlendMode.h>

namespace gum
{

class FastBlendModes
{
public:
	s2::FastBlendMode Name2Mode(const CU_STR& name) const;
	CU_STR Mode2Name(s2::FastBlendMode mode) const;

private:
	 struct Item
	 {
		Item(s2::FastBlendMode mode, const CU_STR& name)
			: mode(mode), name(name) {}

	 	s2::FastBlendMode mode;
	 	CU_STR name;
	 };

private:
	CU_VEC<Item> m_modes;

	CU_SINGLETON_DECLARATION(FastBlendModes);

}; // FastBlendModes

}

#endif // _GUM_FAST_BLEND_MODES_H_
