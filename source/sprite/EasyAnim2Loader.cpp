#include "EasyAnim2Loader.h"
#include "SymbolLoader.h"
#include "SpriteFactory.h"
#include "SkeletonIO.h"

#include <SM_Calc.h>
#include <rigging/rg_skeleton.h>
#include <rigging/rg_joint.h>
#include <sprite2/Anim2Symbol.h>
#include <sprite2/S2_Sprite.h>

#include <queue>

#include <assert.h>

namespace gum
{

EasyAnim2Loader::EasyAnim2Loader(s2::Anim2Symbol* sym, 
								 const SymbolLoader* sym_loader)
	: m_sym(sym)
	, m_sym_loader(sym_loader)
	, m_num(0)
	, m_joint_count(0)
	, m_joints(NULL)
	, m_sk(NULL)
	, m_root(NULL)
{
	if (m_sym) {
		m_sym->AddReference();
	}
	if (m_sym_loader) {
		m_sym_loader->AddReference();
	} else {
		m_sym_loader = new SymbolLoader;
	}
}

EasyAnim2Loader::~EasyAnim2Loader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_sym_loader->RemoveReference();
	Clear();
}

void EasyAnim2Loader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_sym) {
		return;
	}

	Clear();

	m_num = val["skeleton"].size();
	assert(m_num == val["sprite"].size());	
	if (m_num == 0) {
		return;
	}

	LoadSprites(val["sprite"], dir);
	LoadJoints(val["skeleton"]);

	m_sk = (rg_skeleton*)malloc(SIZEOF_RG_SKELETON);
	m_sk->joints = m_joints;
	m_sk->joint_count = m_joint_count;

	InitRoot();
	InitPose();

//	m_sym->SetSkeleton(m_sk);
}

void EasyAnim2Loader::Clear()
{
	m_num = 0;

	for_each(m_sprs.begin(), m_sprs.end(), cu::RemoveRefFunctor<s2::Sprite>());
	m_sprs.clear();

	m_root = NULL;
}

void EasyAnim2Loader::LoadSprites(const Json::Value& val, const std::string& dir)
{
	m_sprs.reserve(m_num);
	for (int i = 0; i < m_num; ++i) {
//		m_sprs.push_back(m_sym_loader->Create(val[i], dir));
	}
}

void EasyAnim2Loader::LoadJoints(const Json::Value& val)
{
	std::vector<Joint> src_joints;
	std::vector<Joint> dst_joints;

	for (int i = 0; i < m_num; ++i) 
	{
		s2::Sprite* spr = m_sprs[i];
		const Json::Value& joints_val = val[i]["joint"];
		bool find = false;
		for (int j = 0, m = joints_val.size(); j < m; ++j) 
		{
			const Json::Value& joint_val = joints_val[j];
			Joint src_joint;
			SkeletonIO::Load(joint_val["world_pose"], src_joint.world_pose);
			SkeletonIO::Load(joint_val["skin_pose"], src_joint.skin_pose);
			src_joint.skin = spr;
			src_joint.idx = i;			
			if (joint_val.isMember("parent")) {
				src_joint.parent = joint_val["parent"].asInt();
				if (!find) {
					find = true;
					dst_joints.push_back(src_joint);
				}
			}
			src_joints.push_back(src_joint);
		}
		if (!find) {
			find = true;
// 			// todo dst_joint
// 			dst_joints.push_back(src_joint);
		}
	}

	// connect
	for (int i = 0, n = src_joints.size(); i < n; ++i) 
	{
		Joint src_joint = src_joints[i];
		if (src_joint.parent == -1) {
			continue;
		}
		int parent_idx = src_joints[src_joint.parent].idx,
			child_idx = src_joint.idx;
		dst_joints[parent_idx].children.push_back(child_idx);
		dst_joints[child_idx].parent = parent_idx;
	}

	// create
	m_joint_count = dst_joints.size();
	m_joints = (rg_joint**)malloc(sizeof(rg_joint*) * m_joint_count);
	for (int i = 0; i < m_joint_count; ++i) 
	{
		const Joint& src = dst_joints[i];
		rg_joint* dst = (rg_joint*)malloc(sizeof(struct rg_joint) + sizeof(uint8_t) * src.children.size());

		dst->name = NULL;

//		CopyJointPose(&dst->skin.local, src.skin_pose);
//		dst->skin.ud = src.skin;

		CopyJointPose(&dst->world_pose, src.world_pose);

		dst->parent = src.parent;
		dst->children_count = src.children.size();
		for (int j = 0; j < dst->children_count; ++j) {
			dst->children[j] = src.children[i];
		}

		m_joints[i] = dst;
	}
}

void EasyAnim2Loader::InitRoot()
{
	assert(m_joint_count > 0);
	m_root = m_joints[0];
	while (m_root->parent != 0xff) {
		m_root = m_joints[m_root->parent];
	}
}

void EasyAnim2Loader::InitPose()
{
	// world
	sm::vec2 root_trans(m_root->world_pose.trans[0], m_root->world_pose.trans[1]);
	for (int i = 0; i < m_root->children_count; ++i)
	{
		rg_joint* child = m_joints[m_root->children[i]];
		sm::vec2 child_trans(child->world_pose.trans[0], child->world_pose.trans[1]);
		float rot = sm::get_line_angle(root_trans, child_trans);
		std::queue<rg_joint*> buf;
		buf.push(child);
		while (!buf.empty()) {
			rg_joint* joint = buf.front(); buf.pop();
			rg_joint_pose* world = &joint->world_pose;
			world->rot += rot;
			for (int i = 0; i < joint->children_count; ++i) {
				buf.push(m_joints[joint->children[i]]);
			}
		}
	}

	// local
	std::queue<rg_joint*> buf;
	for (int i = 0; i < m_root->children_count; ++i) {
		buf.push(m_joints[m_root->children[i]]);
	}
	while (!buf.empty()) {
		rg_joint* joint = buf.front(); buf.pop();
		assert(joint->parent != 0xff);
		rg_joint* parent = m_joints[joint->parent];
		rg_world2local(&parent->world_pose, &joint->world_pose, &joint->local_pose);

// 		const s2::Sprite* skin = joint->GetSkinSpr();
// 		s2::JointPose local(skin->GetCenter(), skin->GetAngle(), skin->GetScale());
// 		joint->SetSkinPose(s2::world2local(joint->GetWorldPose(), local));

// 		const s2::Sprite* spr = joint->skin.ud
// 		rg_world2local();

		for (int i = 0; i < joint->children_count; ++i) {
			buf.push(m_joints[joint->children[i]]);
		}
	}
}

void EasyAnim2Loader::CopyJointPose(rg_joint_pose* dst, const s2::JointPose& src)
{
	dst->trans[0] = src.trans.x;
	dst->trans[1] = src.trans.y;
	dst->rot      = src.rot;
	dst->scale[0] = src.scale.x;
	dst->scale[1] = src.scale.y;
}

}