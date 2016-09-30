#ifndef _GUM_SKELETON_SYM_LOADER_H_
#define _GUM_SKELETON_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/json.h>

#include <string>

namespace s2 { class SkeletonSymbol; }

namespace gum
{

class SkeletonSymLoader : private cu::Uncopyable
{
public:
	SkeletonSymLoader(s2::SkeletonSymbol* sym);
	~SkeletonSymLoader();

	void LoadJson(const std::string& filepath);

private:
	s2::SkeletonSymbol* m_sym;

}; // SkeletonSymLoader

}

#endif // _GUM_SKELETON_SYM_LOADER_H_