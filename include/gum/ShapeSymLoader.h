#ifndef _GUM_SHAPE_SYM_LOADER_H_
#define _GUM_SHAPE_SYM_LOADER_H_

#include <cu/uncopyable.h>

#include <json/json.h>

#include <string>
#include <memory>

namespace s2 { class ShapeSymbol; }
namespace simp { class NodeShape; }

namespace gum
{

class ShapeSymLoader : private cu::Uncopyable
{
public:
	ShapeSymLoader(const std::shared_ptr<s2::ShapeSymbol>& sym);

	void LoadJson(const std::string& filepath);	
	void LoadBin(const simp::NodeShape* node);

private:
	std::shared_ptr<s2::ShapeSymbol> m_sym;

}; // ShapeSymLoader

}

#endif // _GUM_SHAPE_SYM_LOADER_H_