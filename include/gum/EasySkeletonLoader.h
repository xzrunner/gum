#ifndef _GUM_EASY_SKELETON_LOADER_H_
#define _GUM_EASY_SKELETON_LOADER_H_

#include <cu/uncopyable.h>
#include <sprite2/JointPose.h>
#include <sprite2/typedef.h>
#include <cu/cu_stl.h>

#include <json/json.h>

#include <memory>

namespace s2 { class SkeletonSymbol; class Sprite; class Joint; }

namespace gum
{

class SpriteLoader;
class JointLoader;

class EasySkeletonLoader : private cu::Uncopyable
{
public:
	EasySkeletonLoader(const std::shared_ptr<s2::SkeletonSymbol>& sym, 
		const std::shared_ptr<const SpriteLoader>& spr_loader = NULL,
		const std::shared_ptr<const JointLoader>& joint_loader = NULL);
	~EasySkeletonLoader();

	void LoadJson(const Json::Value& val, const CU_STR& dir);

private:
	void Clear();

	void LoadSprites(const Json::Value& val, const CU_STR& dir);
	void LoadJoints(const Json::Value& val);
	void InitRoot();
	void InitPose();

private:
	struct Joint
	{
		s2::JointPose world;
		s2::JointPose skin;
		int parent;
		int idx;

		Joint() : parent(-1), idx(-1) {}
	};

private:
	std::shared_ptr<s2::SkeletonSymbol> m_sym;

	std::shared_ptr<const SpriteLoader> m_spr_loader;
	std::shared_ptr<const JointLoader> m_joint_loader;

	int m_num;
	CU_VEC<s2::SprPtr> m_sprs;
	CU_VEC<std::shared_ptr<s2::Joint>> m_joints;
	std::shared_ptr<s2::Joint> m_root;

}; // EasySkeletonLoader

}

#endif // _GUM_EASY_SKELETON_LOADER_H_