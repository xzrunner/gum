#ifndef _GUM_EASY_ANIM2_LOADER_H_
#define _GUM_EASY_ANIM2_LOADER_H_

#include <CU_Uncopyable.h>
#include <sprite2/JointPose.h>

#include <json/json.h>

struct rg_joint;
struct rg_joint_pose;

namespace s2 { class Anim2Symbol; class Sprite; }

namespace gum
{

class SpriteLoader;
class JointLoader;

class EasyAnim2Loader : private cu::Uncopyable
{
public:
	EasyAnim2Loader(s2::Anim2Symbol* sym, const SpriteLoader* spr_loader = NULL);
	~EasyAnim2Loader();

	void LoadJson(const Json::Value& val, const std::string& dir);

private:
	void Clear();

	void LoadSprites(const Json::Value& val, const std::string& dir);
	void LoadJoints(const Json::Value& val);
	void InitRoot();
	void InitPose();

	static void CopyJointPose(rg_joint_pose* dst, const s2::JointPose& src);

private:
	struct Joint
	{
		s2::JointPose world_pose;
		s2::JointPose skin_pose;

		s2::Sprite* skin;

		int parent;
		std::vector<int> children;

		int idx;

		Joint() : skin(NULL), parent(-1), idx(-1) {}
	};

private:
	s2::Anim2Symbol* m_sym;

	const SpriteLoader* m_spr_loader;

	int m_num;
	std::vector<s2::Sprite*> m_sprs;
	std::vector<rg_joint*> m_joints;
	const rg_joint* m_root;

}; // EasyAnim2Loader

}

#endif // _GUM_EASY_ANIM2_LOADER_H_