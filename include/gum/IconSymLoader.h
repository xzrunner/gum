#ifndef _GUM_ICON_SYM_LOADER_H_
#define _GUM_ICON_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <json/json.h>

namespace s2 { class IconSymbol; }
namespace simp { class NodeIcon; }

namespace gum
{

class IconSymLoader : private cu::Uncopyable
{
public:
	IconSymLoader(s2::IconSymbol& sym);

	void LoadJson(const CU_STR& filepath);	
	void LoadBin(const simp::NodeIcon* node);

private:
	s2::IconSymbol& m_sym;

}; // IconSymLoader

}

#endif // _GUM_ICON_SYM_LOADER_H_