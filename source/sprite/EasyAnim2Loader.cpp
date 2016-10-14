#include "EasyAnim2Loader.h"
#include "SpriteLoader.h"
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
								 const SpriteLoader* spr_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
	, m_num(0)
	, m_root(NULL)
{
	if (m_sym) {
		m_sym->AddReference();
	}
	if (m_spr_loader) {
		m_spr_loader->AddReference();
	} else {
		m_spr_loader = new SpriteLoader;
	}
}

EasyAnim2Loader::~EasyAnim2Loader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_spr_loader->RemoveReference();
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
	InitRoot();
	InitPose();

	m_sym->SetSkeleton(rg_skeleton_create(&m_joints[0], m_joints.size()));
}

void EasyAnim2Loader::Clear()
{
	m_num = 0;

	for_each(m_sprs.begin(), m_sprs.end(), cu::RemoveRefFunctor<s2::Sprite>());
	m_sprs.clear();

	m_joints.clear();

	m_root = NULL;
}

void EasyAnim2Loader::LoadSprites(const Json::Value& val, const std::string& dir)
{
	m_sprs.reserve(m_num);
	for (int i = 0; i < m_num; ++i) {
		m_sprs.push_back(m_spr_loader->Create(val[i], dir));
	}
}

void EasyAnim2Loader::LoadJoints(const Json::Value& val)
{
	m_joints.reserve(m_num);

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
	m_joints.reserve(dst_joints.size());
	for (int i = 0, n = dst_joints.size(); i < n; ++i) 
	{
		const Joint& src = dst_joints[i];
		rg_joint* dst = (rg_joint*)malloc(sizeof(struct rg_joint) + sizeof(uint8_t) * src.children.size());

		dst->name = NULL;

		CopyJointPose(&dst->skin.local, src.skin_pose);
		dst->skin.ud = src.skin;

		CopyJointPose(&dst->world_pose, src.world_pose);

		dst->parent = src.parent;
		dst->children_count = src.children.size();
		for (int j = 0; j < dst->children_count; ++j) {
			dst->children[j] = src.children[i];
		}

		m_joints.push_back(dst);
	}
}

void EasyAnim2Loader::InitRoot()
{
	assert(!m_joints.empty());
	m_root = m_joints[0];

	while (true) {
		int parent = m_root->parent;
		if (parent == -1) {
			break;
		}
		m_root = m_joints[parent];
	}
}

void EasyAnim2Loader::InitPose()
{
	// world
	sm::vec2 root_trans(m_root->world_pose.trans[0], m_root->world_pose.trans[1]);
	for (int i = 0; i < m_root->children_count; ++i)
	{
		rg_joint* child = m_root->children[i];
		sm::vec2 child_trans(child->world_pose.trans[0], child->world_pose.trans[1]);
		float rot = sm::get_line_angle(root_trans, child_trans);
		std::queue<s2::Joint*> buf;
		buf.push(child);
		while (!buf.empty()) {
			s2::Joint* joint = buf.front(); buf.pop();
			s2::JointPose world = joint->GetWorldPose();
			world.rot += rot;
			joint->SetWorldPose(world);
			const std::vector<s2::Joint*>& children = joint->GetChildren();
			for (int i = 0, n = children.size(); i < n; ++i) {
				buf.push(children[i]);
			}
		}
	}

	// local
	std::queue<s2::Joint*> buf;
	for (int i = 0, n = children.size(); i < n; ++i) {
		buf.push(children[i]);
	}
	while (!buf.empty()) {
		s2::Joint* joint = buf.front(); buf.pop();
		assert(joint->GetParent());
		joint->SetLocalPose(s2::world2local(joint->GetParent()->GetWorldPose(), joint->GetWorldPose()));
		const s2::Sprite* skin = joint->GetSkinSpr();
		s2::JointPose local(skin->GetCenter(), skin->GetAngle(), skin->GetScale());
		joint->SetSkinPose(s2::world2local(joint->GetWorldPose(), local));
		const std::vector<s2::Joint*>& children = joint->GetChildren();
		for (int i = 0, n = children.size(); i < n; ++i) {
			buf.push(children[i]);
		}
	}
}

void EasyAnim2Loader::CopyJointPose(rg_joint_pose* dst, const s2::JointPose& src)
{
	dst->trans[0] = src.trans.x;
	dst->trans[1] = src.trans.y;
	dst->rot = src.rot;
	dst->scale[0] = src.scale.x;
	dst->scale[1] = src.scale.y;
}

}