#ifndef _GUM_SPINE_SKELETON_LOADER_H_
#define _GUM_SPINE_SKELETON_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/value.h>

namespace s2 { class SkeletonSymbol; class Sprite; class Joint; }

namespace gum
{

class SpriteLoader;
class JointLoader;
class SpineParser;

class SpineSkeletonLoader : private cu::Uncopyable
{
public:
	SpineSkeletonLoader(s2::SkeletonSymbol* sym, const SpriteLoader* spr_loader = NULL,
		const JointLoader* joint_loader = NULL);
	~SpineSkeletonLoader();

	void LoadJson(const Json::Value& val, const std::string& dir);

	void LoadParser(const SpineParser& parser, const std::string& dir);

private:
	void Clear();

	void LoadSprites(const SpineParser& parser, const std::string& img_dir);
	void LoadJoints(const SpineParser& parser);
	void InitRoot();
	void InitPose(const SpineParser& parser);
	void InitSkeleton(const SpineParser& parser);

private:
	s2::SkeletonSymbol* m_sym;

	const SpriteLoader* m_spr_loader;
	const JointLoader* m_joint_loader;

	int m_num;
	std::vector<s2::Sprite*> m_sprs;
	std::map<std::string, s2::Joint*> m_joints;
	s2::Joint* m_root;

}; // SpineSkeletonLoader

}

#endif // _GUM_SPINE_SKELETON_LOADER_H_