#include "SpineSkeletonLoader.h"
#include "SpriteLoader.h"
#include "JointLoader.h"
#include "SpineParser.h"
#include "FilepathHelper.h"

#include <sprite2/SkeletonSymbol.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/Joint.h>
#include <sprite2/Skeleton.h>

#include <assert.h>

namespace gum
{

SpineSkeletonLoader::SpineSkeletonLoader(s2::SkeletonSymbol* sym, 
										 const SpriteLoader* spr_loader,
										 const JointLoader* joint_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
	, m_joint_loader(joint_loader)
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

SpineSkeletonLoader::~SpineSkeletonLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_spr_loader->RemoveReference();
	m_joint_loader->RemoveReference();
	Clear();
}

void SpineSkeletonLoader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_sym) {
		return;
	}

	SpineParser parser(false);
	parser.Parse(val);
	LoadParser(parser, dir);
}

void SpineSkeletonLoader::LoadParser(const SpineParser& parser, const std::string& dir)
{
	if (!m_sym) {
		return;
	}

	Clear();

	m_num = parser.bones.size();

	std::string img_dir = FilepathHelper::Absolute(dir, parser.img_dir);
	LoadSprites(parser, img_dir);
	LoadJoints(parser);
	InitRoot();
	InitPose(parser);
	InitSkeleton(parser);
}

void SpineSkeletonLoader::Clear()
{
	m_num = 0;

	for (int i = 0, n = m_sprs.size(); i < n; ++i) {
		if (m_sprs[i]) {
			m_sprs[i]->RemoveReference();
		}
	}
	m_sprs.clear();

 	std::map<std::string, s2::Joint*>::iterator itr = m_joints.begin();
 	for ( ; itr != m_joints.end(); ++itr) {
 		itr->second->RemoveReference();
 	}
	m_joints.clear();

	m_root = NULL;
}

void SpineSkeletonLoader::LoadSprites(const SpineParser& parser, const std::string& img_dir)
{
	m_sprs.reserve(m_num);
	for (int i = 0, n = parser.slots.size(); i < n; ++i)
	{
		const SpineParser::Slot& slot = parser.slots[i];
		const SpineParser::SkinItem* item = parser.QuerySkin(slot);
		if (!item) {
			m_sprs.push_back(NULL);
			continue;
		}
		std::string filepath = FilepathHelper::Absolute(img_dir, item->name + ".png");
		s2::Sprite* spr = m_spr_loader->Create(filepath);
		spr->SetAngle(item->angle);
		spr->SetPosition(item->pos);
		m_sprs.push_back(spr);
	}
}

void SpineSkeletonLoader::LoadJoints(const SpineParser& parser)
{
	for (int i = 0, n = parser.slots.size(); i < n; ++i)
	{
		const SpineParser::Slot& slot = parser.slots[i];
		s2::Sprite* spr = m_sprs[i];
		if (!spr) {
			continue;
		}
		s2::JointPose pose(spr->GetPosition(), spr->GetAngle(), spr->GetScale());
		s2::Joint* joint = m_joint_loader->Create(spr, -pose);
		m_joints.insert(std::make_pair(slot.bone, joint));
	}

	for (int i = 0, n = parser.bones.size(); i < n; ++i) {
		const SpineParser::Bone& bone = parser.bones[i];
		std::map<std::string, s2::Joint*>::iterator itr_joint 
			= m_joints.find(bone.name);
		if (itr_joint != m_joints.end()) {
			continue;
		}
		s2::Joint* joint = m_joint_loader->Create(NULL, s2::JointPose());
		m_joints.insert(std::make_pair(bone.name, joint));
	}

	// connect
	for (int i = 0, n = parser.bones.size(); i < n; ++i)
	{
		const SpineParser::Bone& bone = parser.bones[i];
		if (bone.parent.empty()) {
			continue;
		}

		std::map<std::string, s2::Joint*>::iterator itr_joint 
			= m_joints.find(bone.name);
		assert(itr_joint != m_joints.end());
		s2::Joint* child = itr_joint->second;
		itr_joint = m_joints.find(bone.parent);
		assert(itr_joint != m_joints.end());
		s2::Joint* parent = itr_joint->second;
		parent->ConnectChild(child);
	}
}

void SpineSkeletonLoader::InitRoot()
{
	assert(!m_joints.empty());
	const s2::Joint* root = m_joints.begin()->second;
	while (const s2::Joint* parent = root->GetParent()) {
		root = parent;
	}
	m_root = const_cast<s2::Joint*>(root);
}

void SpineSkeletonLoader::InitPose(const SpineParser& parser)
{
	std::map<std::string, s2::Joint*>::iterator itr = m_joints.begin();
	for ( ; itr != m_joints.end(); ++itr)
	{
		const SpineParser::Bone* src = NULL;
		for (int i = 0, n = parser.bones.size(); i < n; ++i) {
			if (parser.bones[i].name == itr->first) {
				src = &parser.bones[i];
				break;
			}			
		}
		assert(src);

		s2::Joint* dst = itr->second;
		dst->SetLocalPose(s2::JointPose(src->pos, src->angle, src->scale));
	}
	m_root->Update();
}

void SpineSkeletonLoader::InitSkeleton(const SpineParser& parser)
{
	std::vector<s2::Joint*> joints;
	joints.reserve(m_joints.size());
	for (int i = 0, n = parser.slots.size(); i < n; ++i) 
	{
		std::map<std::string, s2::Joint*>::iterator itr
			= m_joints.find(parser.slots[i].bone);
		assert(itr != m_joints.end());
		s2::Joint* joint = itr->second;
		if (joint->GetName().empty()) {
			joint->SetName("*");
			joints.push_back(joint);
		}
	}

	std::map<std::string, s2::Joint*>::iterator itr 
		= m_joints.begin();
	for ( ; itr != m_joints.end(); ++itr) 
	{
		s2::Joint* joint = itr->second;
		if (joint->GetName().empty()) {
			joints.push_back(joint);
		}
		joint->SetName(itr->first);
	}

	m_sym->SetSkeleton(new s2::Skeleton(m_root, joints));
}

}