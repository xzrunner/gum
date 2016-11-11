#include "SpineAnim2Loader.h"
#include "SymbolLoader.h"
#include "SpineParser.h"
#include "FilepathHelper.h"

#include <sprite2/Anim2Symbol.h>
#include <sprite2/S2_Sprite.h>
#include <rigging/rg_joint.h>
#include <rigging/rg_skeleton.h>

#include <assert.h>

namespace gum
{

SpineAnim2Loader::SpineAnim2Loader(s2::Anim2Symbol* sym, 
								   const SymbolLoader* sym_loader)
	: m_sym(sym)
	, m_spr_loader(sym_loader)
	, m_num(0)
	, m_joint_count(0)
	, m_joints(NULL)
	, m_sk(NULL)
	, m_root(NULL)
{
	if (m_sym) {
		m_sym->AddReference();
	}
	if (m_spr_loader) {
		m_spr_loader->AddReference();
	} else {
		m_spr_loader = new SymbolLoader;
	}
}

SpineAnim2Loader::~SpineAnim2Loader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_spr_loader->RemoveReference();
	Clear();
}

void SpineAnim2Loader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_sym) {
		return;
	}

	SpineParser parser(false);
	parser.Parse(val);
	LoadParser(parser, dir);
}

void SpineAnim2Loader::LoadParser(const SpineParser& parser, const std::string& dir)
{
	if (!m_sym) {
		return;
	}

	Clear();

	m_num = parser.bones.size();

	std::string img_dir = FilepathHelper::Absolute(dir, parser.img_dir);

	LoadSprites(parser, img_dir);

	LoadJointsData(parser);
	ConnectJoints(parser);

	CreateJoints();
	CreateSkeleton();

	InitRoot();
	InitPose(parser);
}

void SpineAnim2Loader::Clear()
{
	m_num = 0;

	for (int i = 0, n = m_syms.size(); i < n; ++i) {
		if (m_syms[i]) {
			m_syms[i]->RemoveReference();
		}
	}
	m_syms.clear();

//	m_root = NULL;
}

void SpineAnim2Loader::LoadSprites(const SpineParser& parser, const std::string& img_dir)
{
	m_syms.reserve(m_num);
	for (int i = 0, n = parser.slots.size(); i < n; ++i)
	{
		const SpineParser::Slot& slot = parser.slots[i];
		const SpineParser::SkinItem* item = parser.QuerySkin(slot);
		if (!item) {
			m_syms.push_back(NULL);
			continue;
		}
		std::string filepath = FilepathHelper::Absolute(img_dir, item->name + ".png");
		s2::Symbol* sym = m_spr_loader->Create(filepath);
// 		spr->SetAngle(item->angle);
// 		spr->SetPosition(item->pos);
		m_syms.push_back(sym);
	}
}

void SpineAnim2Loader::LoadJointsData(const SpineParser& parser)
{
	for (int i = 0, n = parser.slots.size(); i < n; ++i)
	{
		const SpineParser::Slot& slot = parser.slots[i];
		const SpineParser::SkinItem* item = parser.QuerySkin(slot);
		s2::Symbol* sym = m_syms[i];
		if (!sym || !item) {
			continue;
		}

		JointData data;
		data.name          = slot.bone;
		data.skin          = sym;
		data.pose.trans[0] = item->pos.x;
		data.pose.trans[1] = item->pos.y;
		data.pose.rot      = item->angle;
		data.pose.scale[0] = 1;
		data.pose.scale[1] = 1;

		m_map2jointdata.insert(std::make_pair(slot.bone, m_joints_data.size()));
		m_joints_data.push_back(data);
	}

	std::map<std::string, SpineParser::Bone>::const_iterator itr 
		= parser.bones.begin();
	for ( ; itr != parser.bones.end(); ++itr)
	{
		const SpineParser::Bone& bone = itr->second;
		std::map<std::string, int>::iterator itr_joint 
			= m_map2jointdata.find(bone.name);
		if (itr_joint != m_map2jointdata.end()) {
			continue;
		}

		JointData data;
		data.name = bone.name;
		m_map2jointdata.insert(std::make_pair(bone.name, m_joints_data.size()));
		m_joints_data.push_back(data);
	}
}

void SpineAnim2Loader::ConnectJoints(const SpineParser& parser)
{
	std::map<std::string, SpineParser::Bone>::const_iterator itr 
		= parser.bones.begin();
	for ( ; itr != parser.bones.end(); ++itr)
	{
		const SpineParser::Bone& bone = itr->second;
		if (bone.parent.empty()) {
			continue;
		}

		std::map<std::string, int>::iterator itr_joint 
			= m_map2jointdata.find(bone.name);
		assert(itr_joint != m_map2jointdata.end());
		int child = itr_joint->second;
		itr_joint = m_map2jointdata.find(bone.parent);
		assert(itr_joint != m_map2jointdata.end());
		int parent = itr_joint->second;

		bool find = false;
		JointData& data = m_joints_data[parent];
		for (int i = 0, n = data.children.size(); i < n; ++i) {
			if (data.children[i] == child) {
				find = true;
				break;
			}
		}
		if (!find) {
			data.children.push_back(child);
		}
	}
}

void SpineAnim2Loader::CreateJoints()
{
	m_joint_count = m_joints_data.size();
	m_joints = (rg_joint**)malloc(sizeof(rg_joint*) * m_joints_data.size());
	for (int i = 0, n = m_joints_data.size(); i < n; ++i) 
	{
		const JointData& src = m_joints_data[i];
		rg_joint* dst = (rg_joint*)malloc(SIZEOF_RG_JOINT + sizeof(uint8_t) * src.children.size());
		dst->skin.ud        = src.skin;
		dst->skin.local     = src.pose;
		dst->parent         = 0xff;
		dst->children_count = src.children.size();
		for (int j = 0; j < dst->children_count; ++j) {
			dst->children[j] = src.children[j];
		}
		m_joints[i] = dst;
	}
	for (int i = 0, n = m_joints_data.size(); i < n; ++i) {
		const JointData& src = m_joints_data[i];
		for (int j = 0, m = src.children.size(); j < m; ++j) {
			m_joints[src.children[j]]->parent = i;
		}
	}
}

void SpineAnim2Loader::CreateSkeleton()
{
	m_sk = (rg_skeleton*)malloc(SIZEOF_RG_SKELETON);
	m_sk->joints = m_joints;
	m_sk->joint_count = m_joint_count;
	m_sym->SetSkeleton(m_sk);
}

void SpineAnim2Loader::InitRoot()
{
	assert(m_joint_count > 0);
	m_root = m_joints[0];
	while (m_root->parent != 0xff) {
		m_root = m_joints[m_root->parent];
	}
}

void SpineAnim2Loader::InitPose(const SpineParser& parser)
{
	for (int i = 0; i < m_joint_count; ++i) 
	{
		std::map<std::string, SpineParser::Bone>::const_iterator itr_bone 
			= parser.bones.find(m_joints_data[i].name);
		const SpineParser::Bone& src = itr_bone->second;

		rg_joint* dst = m_joints[i];

		dst->local_pose.trans[0] = src.pos.x;
		dst->local_pose.trans[1] = src.pos.y;
		dst->local_pose.rot      = src.angle;
		dst->local_pose.scale[0] = src.scale.x;
		dst->local_pose.scale[1] = src.scale.y;

		rg_joint_pose_identity(&dst->world_pose);
	}
	rg_joint_update(m_root, m_sk);
}

/************************************************************************/
/* class SpineAnim2Loader::JointData                                    */
/************************************************************************/

SpineAnim2Loader::JointData::JointData()
	: skin(NULL)
{
	memset(&pose, 0, sizeof(pose));
}

}