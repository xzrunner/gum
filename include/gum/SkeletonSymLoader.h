#ifndef _GUM_SKELETON_SYM_LOADER_H_
#define _GUM_SKELETON_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <SM_Vector.h>

#include <json/json.h>

#include <string>

namespace s2 { class SkeletonSymbol; class Joint; class Sprite; class JointPose; }

namespace gum
{

class SpriteLoader;
class JointLoader;

class SkeletonSymLoader : private cu::Uncopyable
{
public:
	SkeletonSymLoader(s2::SkeletonSymbol* sym, const SpriteLoader* spr_loader = NULL,
		const JointLoader* joint_loader = NULL);
	~SkeletonSymLoader();

	void LoadJson(const std::string& filepath);

private:
	s2::SkeletonSymbol* m_sym;

	const SpriteLoader* m_spr_loader;
	const JointLoader* m_joint_loader;

}; // SkeletonSymLoader

}

#endif // _GUM_SKELETON_SYM_LOADER_H_