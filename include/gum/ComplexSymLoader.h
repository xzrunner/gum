#ifndef _GUM_COMPLEX_SYM_LOADER_H_
#define _GUM_COMPLEX_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <json/json.h>

#include <memory>

namespace s2 { class ComplexSymbol; }
namespace simp { class NodeComplex; }

namespace gum
{

class ComplexSymLoader : private cu::Uncopyable
{
public:
	ComplexSymLoader(const std::shared_ptr<s2::ComplexSymbol>& sym);

	void LoadJson(const CU_STR& filepath);
	void LoadBin(const simp::NodeComplex* node);

public:
	struct Action
	{
		CU_STR name;
		CU_VEC<int> idx;
	};

// 	static void LoadJsonAction(const Json::Value& val, s2::ComplexSymbol* sym);
// 	static void LoadJsonAction(const Json::Value& val, CU_VEC<Action>& actions);

private:
	std::shared_ptr<s2::ComplexSymbol> m_sym;

}; // ComplexSymLoader

}

#endif // _GUM_COMPLEX_SYM_LOADER_H_
