#ifndef _GUM_EASY_SKELETON_LOADER_H_
#define _GUM_EASY_SKELETON_LOADER_H_

#include <CU_Uncopyable.h>
#include <sprite2/JointPose.h>

#include <json/json.h>

namespace s2 { class SkeletonSymbol; class Sprite; class Joint; }

namespace gum
{

class SpriteLoader;
class JointLoader;

class EasySkeletonLoader : private cu::Uncopyable
{
public:
	EasySkeletonLoader(s2::SkeletonSymbol* sym, const SpriteLoader* spr_loader = NULL,
		const JointLoader* joint_loader = NULL);
	~EasySkeletonLoader();

	void LoadJson(const Json::Value& val, const std::string& dir);

private:
	void Clear();

	void LoadSprites(const Json::Value& val, const std::string& dir);
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
	s2::SkeletonSymbol* m_sym;

	const SpriteLoader* m_spr_loader;
	const JointLoader* m_joint_loader;

	int m_num;
	std::vector<s2::Sprite*> m_sprs;
	std::vector<s2::Joint*> m_joints;
	const s2::Joint* m_root;

}; // EasySkeletonLoader

}

#endif // _GUM_EASY_SKELETON_LOADER_H_