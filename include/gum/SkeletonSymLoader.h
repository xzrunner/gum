#ifndef _GUM_SKELETON_SYM_LOADER_H_
#define _GUM_SKELETON_SYM_LOADER_H_

#include <CU_Uncopyable.h>

#include <SM_Vector.h>

#include <json/json.h>

#include <string>

namespace s2 { class SkeletonSymbol; class Joint; class Sprite; class JointPose; }

namespace gum
{

class SkeletonSymLoader : private cu::Uncopyable
{
public:
	SkeletonSymLoader(s2::SkeletonSymbol* sym);
	~SkeletonSymLoader();

	void LoadJson(const std::string& filepath);

protected:
	virtual s2::Joint* CreateJoint(s2::Sprite* spr, const s2::JointPose& joint_pose) const;

private:
	s2::SkeletonSymbol* m_sym;

}; // SkeletonSymLoader

}

#endif // _GUM_SKELETON_SYM_LOADER_H_