#include "EasySkeletonLoader.h"
#include "SpriteLoader.h"
#include "JointLoader.h"
#include "SpriteFactory.h"
#include "SkeletonIO.h"

#include <SM_Calc.h>
#include <sprite2/SkeletonSymbol.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/Joint.h>
#include <sprite2/Skeleton.h>

#include <queue>

#include <assert.h>

namespace gum
{

EasySkeletonLoader::EasySkeletonLoader(s2::SkeletonSymbol* sym, 
									   const SpriteLoader* spr_loader,
									   const JointLoader* joint_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
	, m_joint_loader(joint_loader)
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
	if (m_joint_loader) {
		m_joint_loader->AddReference();
	} else {
		m_joint_loader = new JointLoader;
	}
}

EasySkeletonLoader::~EasySkeletonLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_spr_loader->RemoveReference();
	m_joint_loader->RemoveReference();
	Clear();
}

void EasySkeletonLoader::LoadJson(const Json::Value& val, const std::string& dir)
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

	m_sym->SetSkeleton(new s2::Skeleton(m_root, m_joints));
}

void EasySkeletonLoader::Clear()
{
	m_num = 0;

	for_each(m_sprs.begin(), m_sprs.end(), cu::RemoveRefFunctor<s2::Sprite>());
	m_sprs.clear();

	for_each(m_joints.begin(), m_joints.end(), cu::RemoveRefFunctor<s2::Joint>());
	m_joints.clear();

	m_root = NULL;
}

void EasySkeletonLoader::LoadSprites(const Json::Value& val, const std::string& dir)
{
	m_sprs.reserve(m_num);
	for (int i = 0; i < m_num; ++i) {
		m_sprs.push_back(m_spr_loader->Create(val[i], dir));
	}
}

void EasySkeletonLoader::LoadJoints(const Json::Value& val)
{
	m_joints.reserve(m_num);

	std::vector<Joint> src_joints;
	for (int i = 0; i < m_num; ++i) 
	{
		s2::Sprite* spr = m_sprs[i];
		const Json::Value& joints_val = val[i]["joint"];
		s2::Joint* dst_joint = NULL;
		for (int j = 0, m = joints_val.size(); j < m; ++j) 
		{
			const Json::Value& joint_val = joints_val[j];
			Joint src_joint;
			SkeletonIO::Load(joint_val["world_pose"], src_joint.world);
			SkeletonIO::Load(joint_val["skin_pose"], src_joint.skin);
			src_joint.idx = i;
			if (joint_val.isMember("parent")) {
				src_joint.parent = joint_val["parent"].asInt();
				if (!dst_joint) {
					dst_joint = m_joint_loader->Create(spr, -src_joint.skin);
					dst_joint->SetWorldPose(src_joint.world);
				}
			}
			src_joints.push_back(src_joint);
		}
		if (!dst_joint) {
			dst_joint = m_joint_loader->Create(spr, s2::JointPose());
			dst_joint->SetWorldPose(s2::JointPose(spr->GetCenter(), spr->GetAngle()));
		}
		m_joints.push_back(dst_joint);
	}

	// connect
	for (int i = 0, n = src_joints.size(); i < n; ++i) {
		Joint src_joint = src_joints[i];
		if (src_joint.parent != -1) {
			m_joints[src_joints[src_joint.parent].idx]->ConnectChild(m_joints[src_joint.idx]);
		}
	}
}

void EasySkeletonLoader::InitRoot()
{
	assert(!m_joints.empty());
	m_root = m_joints[0];
	while (const s2::Joint* parent = m_root->GetParent()) {
		m_root = parent;
	}
}

void EasySkeletonLoader::InitPose()
{
	// world
	std::vector<s2::Joint*> children = m_root->GetChildren();
	for (int i = 0, n = children.size(); i < n; ++i) {
		s2::Joint* child = children[i];
		float rot = sm::get_line_angle(m_root->GetWorldPose().trans, child->GetWorldPose().trans);
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
		joint->SetSkinPose(s2::world2local(joint->GetWorldPose(), s2::JointPose(skin->GetCenter(), skin->GetAngle())));
		const std::vector<s2::Joint*>& children = joint->GetChildren();
		for (int i = 0, n = children.size(); i < n; ++i) {
			buf.push(children[i]);
		}
	}
}

}