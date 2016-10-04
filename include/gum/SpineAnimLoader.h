#ifndef _GUM_SPINE_ANIM_LOADER_H_
#define _GUM_SPINE_ANIM_LOADER_H_

#include "SpineParser.h"

#include <CU_Uncopyable.h>
#include <sprite2/JointPose.h>

#include <json/json.h>

#include <string>

namespace s2 { class AnimSymbol; class SkeletonPose; class SkeletonSymbol; }

namespace gum
{

class SymbolLoader;
class SpriteLoader;

class SpineAnimLoader : private cu::Uncopyable
{
public:
	SpineAnimLoader(s2::AnimSymbol* sym, const SymbolLoader* sym_loader = NULL,
		const SpriteLoader* spr_loader = NULL);
	~SpineAnimLoader();

	void LoadJson(const Json::Value& val, const std::string& dir,
		const std::string& filepath);

private:
	void BuildBone2PoseTable();

	float GetNextTime();
	void LoadJointPoses(float next_time, s2::SkeletonPose& sk_pose);
	void UpdateNextTime(float next_time);

private:
	s2::AnimSymbol* m_sym;

	const SymbolLoader* m_sym_loader;
	const SpriteLoader* m_spr_loader;

	const SpineParser::Animation* m_src_anim;

	s2::SkeletonSymbol* m_sk_sym;

	// table for bone to pose index
	std::vector<int> m_bone2pose;

	// index to current each pose
	std::vector<int> m_pose_ptrs;

}; // SpineAnimLoader

}

#endif // _GUM_SPINE_ANIM_LOADER_H_