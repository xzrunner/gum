#ifndef _GUM_SHAPE_SYM_LOADER_H_
#define _GUM_SHAPE_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/json.h>

#include <string>

namespace s2 { class ShapeSymbol; }
namespace simp { class NodeShape; }

namespace gum
{

class ShapeSymLoader : private cu::Uncopyable
{
public:
	ShapeSymLoader(s2::ShapeSymbol* sym, bool flatten);
	~ShapeSymLoader();

	void LoadJson(const std::string& filepath);	
	void LoadBin(const simp::NodeShape* node);

private:
	s2::ShapeSymbol* m_sym;

	bool m_flatten;

}; // ShapeSymLoader

}

#endif // _GUM_SHAPE_SYM_LOADER_H_