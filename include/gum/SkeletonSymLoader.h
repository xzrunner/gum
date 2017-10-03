#ifndef _GUM_SKELETON_SYM_LOADER_H_
#define _GUM_SKELETON_SYM_LOADER_H_

#include <cu/uncopyable.h>

#include <SM_Vector.h>

#include <json/json.h>

#include <string>
#include <memory>

namespace s2 { class SkeletonSymbol; class Joint; class Sprite; class JointPose; }

namespace gum
{

class SpriteLoader;
class JointLoader;

class SkeletonSymLoader : private cu::Uncopyable
{
public:
	SkeletonSymLoader(const std::shared_ptr<s2::SkeletonSymbol>& sym, 
		const std::shared_ptr<const SpriteLoader>& spr_loader = NULL,
		const std::shared_ptr<const JointLoader>& joint_loader = NULL);

	void LoadJson(const std::string& filepath);

private:
	std::shared_ptr<s2::SkeletonSymbol> m_sym;

	std::shared_ptr<const SpriteLoader> m_spr_loader;
	std::shared_ptr<const JointLoader>  m_joint_loader;

}; // SkeletonSymLoader

}

#endif // _GUM_SKELETON_SYM_LOADER_H_