#ifndef _GUM_ICON_SYM_LOADER_H_
#define _GUM_ICON_SYM_LOADER_H_

#include <cu/uncopyable.h>

#include <json/json.h>

#include <string>
#include <memory>

namespace s2 { class IconSymbol; }
namespace simp { class NodeIcon; }

namespace gum
{

class IconSymLoader : private cu::Uncopyable
{
public:
	IconSymLoader(const std::shared_ptr<s2::IconSymbol>& sym);

	void LoadJson(const std::string& filepath);	
	void LoadBin(const simp::NodeIcon* node);

private:
	std::shared_ptr<s2::IconSymbol> m_sym;

}; // IconSymLoader

}

#endif // _GUM_ICON_SYM_LOADER_H_