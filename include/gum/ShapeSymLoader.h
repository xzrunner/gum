#ifndef _GUM_SHAPE_SYM_LOADER_H_
#define _GUM_SHAPE_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <json/json.h>

namespace s2 { class ShapeSymbol; }
namespace simp { class NodeShape; }

namespace gum
{

class ShapeSymLoader : private cu::Uncopyable
{
public:
	ShapeSymLoader(s2::ShapeSymbol& sym);

	void LoadJson(const CU_STR& filepath);	
	void LoadBin(const simp::NodeShape* node);

private:
	s2::ShapeSymbol& m_sym;

}; // ShapeSymLoader

}

#endif // _GUM_SHAPE_SYM_LOADER_H_