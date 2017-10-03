#ifndef _GUM_COMPLEX_SYM_LOADER_H_
#define _GUM_COMPLEX_SYM_LOADER_H_

#include <cu/uncopyable.h>

#include <json/json.h>

#include <string>
#include <memory>

namespace s2 { class ComplexSymbol; }
namespace simp { class NodeComplex; }

namespace gum
{

class ComplexSymLoader : private cu::Uncopyable
{
public:
	ComplexSymLoader(const std::shared_ptr<s2::ComplexSymbol>& sym);

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeComplex* node);

public:
	struct Action
	{
		std::string name;
		std::vector<int> idx;
	};

// 	static void LoadJsonAction(const Json::Value& val, s2::ComplexSymbol* sym);
// 	static void LoadJsonAction(const Json::Value& val, std::vector<Action>& actions);

private:
	std::shared_ptr<s2::ComplexSymbol> m_sym;

}; // ComplexSymLoader

}

#endif // _GUM_COMPLEX_SYM_LOADER_H_
