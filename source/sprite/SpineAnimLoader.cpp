#include "SpineAnimLoader.h"
#include "SpineParser.h"
#include "SymbolLoader.h"
#include "SpriteLoader.h"
#include "FilepathHelper.h"

#include <sprite2/S2_Sprite.h>
#include <sprite2/AnimSymbol.h>
#include <sprite2/SkeletonSymbol.h>
#include <sprite2/SkeletonSprite.h>
#include <sprite2/Joint.h>
#include <sprite2/Skeleton.h>

#include <assert.h>

namespace gum
{

static const float FPS = 30.0f;
static const int DEFAULT_ANIM = 0;

SpineAnimLoader::SpineAnimLoader(s2::AnimSymbol* sym, 
								 const SymbolLoader* sym_loader,
								 const SpriteLoader* spr_loader)
	: m_sym(sym)
	, m_sym_loader(sym_loader)
	, m_spr_loader(spr_loader)
	, m_sk_sym(NULL)
	, m_src_anim(NULL)
{
	if (m_sym) {
		m_sym->AddReference();
	}
	if (m_sym_loader) {
		m_sym_loader->AddReference();
	} else {
		m_sym_loader = new SymbolLoader;
	}
	if (m_spr_loader) {
		m_spr_loader->AddReference();
	} else {
		m_spr_loader = new SpriteLoader;
	}
}

SpineAnimLoader::~SpineAnimLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_sym_loader->RemoveReference();
	m_spr_loader->RemoveReference();
	if (m_sk_sym) {
		m_sk_sym->RemoveReference();
	}
}

void SpineAnimLoader::LoadJson(const Json::Value& val, const std::string& dir,
							   const std::string& filepath)
{
	m_sym->SetFPS(static_cast<int>(FPS));

	SpineParser parser;
	parser.Parse(val);

	assert(!parser.anims.empty());
	m_src_anim = &parser.anims[DEFAULT_ANIM];

	m_sk_sym = VI_DOWNCASTING<s2::SkeletonSymbol*>(m_sym_loader->Create(filepath, s2::SYM_SKELETON));
	
	BuildBone2PoseTable();

	s2::AnimSymbol::Layer* layer = new s2::AnimSymbol::Layer;
	
	int bone_num = m_src_anim->bones.size();
	m_pose_ptrs.resize(bone_num * 3, 0);
	int next_time = 0;
	while (true)
	{
		s2::AnimSymbol::Frame* frame = new s2::AnimSymbol::Frame;
		frame->index = next_time + 1;
		frame->tween = true;
		s2::Sprite* spr = m_spr_loader->Create(m_sk_sym);
		LoadJointPoses(next_time, VI_DOWNCASTING<s2::SkeletonSprite*>(spr)->GetPose());
		frame->sprs.push_back(spr);
		layer->frames.push_back(frame);
		if (!UpdateNextTime(next_time)) {
			break;
		}
	}

	m_sym->AddLayer(layer);
}

void SpineAnimLoader::BuildBone2PoseTable()
{
	m_bone2pose.reserve(m_src_anim->bones.size());
	const s2::Skeleton* sk = m_sk_sym->GetSkeleton();
	const std::vector<s2::Joint*>& all_joints = sk->GetAllJoints();
	for (int i = 0, n = m_src_anim->bones.size(); i < n; ++i) 
	{
		const std::string& name = m_src_anim->bones[i].name;
		int idx = -1;
		for (int j = 0, m = all_joints.size(); j < m; ++j) {
			if (all_joints[j]->GetName() == name) {
				idx = j;
				break;
			}
		}
		assert(idx != -1);
		m_bone2pose.push_back(idx);
	}
}

bool SpineAnimLoader::UpdateNextTime(int& next_time)
{
	int bone_num = m_src_anim->bones.size();

	// next time
	int ptr = 0;
	for (int i = 0; i < bone_num; ++i)
	{
		const SpineParser::AnimBone& src_bone = m_src_anim->bones[i];
		// rotate
		int time = int(src_bone.rotates[m_pose_ptrs[ptr++]].time * FPS);
		if (time < next_time) next_time = time;
		// translate
		time = int(src_bone.translates[m_pose_ptrs[ptr++]].time * FPS);
		if (time < next_time) next_time = time;			
		// scale
		time = int(src_bone.scales[m_pose_ptrs[ptr++]].time * FPS);
		if (time < next_time) next_time = time;
	}

	// update
	bool dirty = false;
	ptr = 0;
	for (int i = 0; i < bone_num; ++i)
	{
		const SpineParser::AnimBone& src_bone = m_src_anim->bones[i];
		// rotate
		int idx = m_pose_ptrs[ptr++];
		int time = int(src_bone.rotates[idx].time / FPS);
		if (time == next_time && idx < src_bone.rotates.size() - 1) {
			++m_pose_ptrs[ptr - 1];
			dirty = true;
		}
		// translate
		idx = m_pose_ptrs[ptr++];
		time = int(src_bone.translates[idx].time * FPS);
		if (time == next_time && idx < src_bone.translates.size() - 1) {
			++m_pose_ptrs[ptr - 1];
			dirty = true;
		}
		// scale 
		idx = m_pose_ptrs[ptr++];
		time = int(src_bone.scales[idx].time * FPS);
		if (time == next_time && idx < src_bone.scales.size() - 1) {
			++m_pose_ptrs[ptr - 1];
			dirty = true;
		}
	}

	return dirty;
}

void SpineAnimLoader::LoadJointPoses(int next_time, s2::SkeletonPose& sk_pose)
{
	float time = next_time * FPS;
	int ptr = 0;
	int bone_num = m_src_anim->bones.size();
	for (int i = 0; i < bone_num; ++i)
	{
		const SpineParser::AnimBone& src_bone = m_src_anim->bones[i];
		s2::JointPose pose;

		// rotate
		int idx = m_pose_ptrs[ptr++];
		if (idx < src_bone.rotates.size() - 1) {
			float t0 = src_bone.rotates[idx].time,
				  t1 = src_bone.rotates[idx + 1].time;
			if (time >= t0) 
			{
				assert(time <= t1);
				float v0 = src_bone.rotates[idx].rot,
					  v1 = src_bone.rotates[idx + 1].rot;
				pose.rot = (time - t0) * (v1 - v0) / (t1 - t0) + v0;
			}
		} else {
			pose.rot = src_bone.rotates[idx].rot;
		}
		// translate
		idx = m_pose_ptrs[ptr++];
		if (idx < src_bone.translates.size() - 1) {
			float t0 = src_bone.translates[idx].time,
				  t1 = src_bone.translates[idx + 1].time;
			if (time > t0) 
			{
				assert(time <= t1);
				sm::vec2 v0 = src_bone.translates[idx].trans,
					     v1 = src_bone.translates[idx + 1].trans;
				pose.trans = (v1 - v0) * (time - t0) / (t1 - t0) + v0;
			}
		} else {
			pose.trans = src_bone.translates[idx].trans;
		}
		// scale
		idx = m_pose_ptrs[ptr++];
		if (idx < src_bone.scales.size() - 1) {
			float t0 = src_bone.scales[idx].time,
				  t1 = src_bone.scales[idx + 1].time;
			if (time > t0) 
			{
				assert(time <= t1);
				sm::vec2 v0 = src_bone.scales[idx].scale,
					     v1 = src_bone.scales[idx + 1].scale;
				pose.trans = (v1 - v0) * (time - t0) / (t1 - t0) + v0;
			}
		} else {
			pose.trans = src_bone.scales[idx].scale;
		}

		sk_pose.SetJointPose(m_bone2pose[i], pose);
	}
}

}