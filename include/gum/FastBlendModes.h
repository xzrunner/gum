#ifndef _GUM_FAST_BLEND_MODES_H_
#define _GUM_FAST_BLEND_MODES_H_

#include <cu/cu_macro.h>

#include <sprite2/FastBlendMode.h>

#include <vector>
#include <string>

namespace gum
{

class FastBlendModes
{
public:
	s2::FastBlendMode Name2Mode(const std::string& name) const;
	std::string Mode2Name(s2::FastBlendMode mode) const;

private:
	 struct Item
	 {
		Item(s2::FastBlendMode mode, const std::string& name)
			: mode(mode), name(name) {}

	 	s2::FastBlendMode mode;
	 	std::string name;
	 };

private:
	std::vector<Item> m_modes;

	CU_SINGLETON_DECLARATION(FastBlendModes);

}; // FastBlendModes

}

#endif // _GUM_FAST_BLEND_MODES_H_
