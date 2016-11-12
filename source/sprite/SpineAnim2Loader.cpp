#include "SpineAnim2Loader.h"
#include "SymbolLoader.h"
#include "SpineParser.h"
#include "FilepathHelper.h"

#include <sprite2/Anim2Symbol.h>
#include <sprite2/S2_Sprite.h>
#include <rigging/rg_joint.h>
#include <rigging/rg_skeleton.h>
#include <rigging/rg_dopesheet.h>
#include <rigging/rg_animation.h>

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
	, m_sheets(NULL)
	, m_max_frame(0)
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

	SpineParser parser(true);
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

	LoadDopesheets(parser);

	rg_animation* anim = (rg_animation*)malloc(SIZEOF_RG_ANIM);
	anim->sk = m_sk;
	anim->ds = m_sheets;
	anim->max_frame = m_max_frame;
	m_sym->SetAnim(anim);
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
		dst->name           = strdup(src.name.c_str());
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
}

void SpineAnim2Loader::InitRoot()
{
	assert(m_joint_count > 0);
	m_sk->root = 0;
	m_root = m_joints[0];
	while (m_root->parent != 0xff) {
		m_sk->root = m_root->parent;
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

void SpineAnim2Loader::LoadDopesheets(const SpineParser& parser)
{
	static const int FPS = 30;

	static const int ANIM_IDX = 0;
	const SpineParser::Animation& anim = parser.anims[ANIM_IDX];

	m_sheets = (rg_dopesheet**)malloc(sizeof(struct rg_dopesheet*) * m_sk->joint_count);
	for (int i = 0; i < m_sk->joint_count; ++i) 
	{
		const std::string& name = m_joints_data[i].name;
		const SpineParser::AnimBone* bone = NULL;
		for (int j = 0, m = anim.bones.size(); j < m; ++j) {
			if (name == anim.bones[j].name) {
				bone = &anim.bones[j];
				break;
			}
		}
		assert(bone);

		int frame_count = bone->translates.size() * 2 + bone->rotates.size() + bone->scales.size() * 2;
		int sz = SIZEOF_RG_DOPESHEET + sizeof(struct rg_frame) * frame_count;
		struct rg_dopesheet* ds = (struct rg_dopesheet*)malloc(sz);
		memset(ds, 0, sz);

		int frame_ptr = 0;
		if (!bone->translates.empty())
		{
			ds->type |= DIM_FLAG_TRANS_X;
			ds->type |= DIM_FLAG_TRANS_Y;
			ds->dims_count[DIM_IDX_TRANS_X] = ds->dims_count[DIM_IDX_TRANS_Y] = bone->translates.size();

			int max_frame = (int)(bone->translates.back().time * 30 + 0.5f);
			if (max_frame > m_max_frame) {
				m_max_frame = max_frame;
			}

			rg_frame* frame = &ds->frames[frame_ptr];
			for (int i = 0, n = bone->translates.size(); i < n; ++i) {
				frame[i].time = (int)(bone->translates[i].time * 30 + 0.5f);
				frame[i].lerp = 0;
				frame[i].data = bone->translates[i].trans.x;
			}
			frame_ptr += bone->translates.size();

			frame = &ds->frames[frame_ptr];
			for (int i = 0, n = bone->translates.size(); i < n; ++i) {
				frame[i].time = (int)(bone->translates[i].time * 30 + 0.5f);
				frame[i].lerp = 0;
				frame[i].data = bone->translates[i].trans.y;
			}
			frame_ptr += bone->translates.size();
		}
		if (!bone->rotates.empty())
		{
			ds->type |= DIM_FLAG_ROT;
			ds->dims_count[DIM_IDX_ROT] = bone->rotates.size();

			int max_frame = (int)(bone->rotates.back().time * 30 + 0.5f);
			if (max_frame > m_max_frame) {
				m_max_frame = max_frame;
			}

			rg_frame* frame = &ds->frames[frame_ptr];
			for (int i = 0, n = bone->rotates.size(); i < n; ++i) {
				frame[i].time = (int)(bone->rotates[i].time * 30 + 0.5f);
				frame[i].lerp = 0;
				frame[i].data = bone->rotates[i].rot;
			}
			frame_ptr += bone->rotates.size();
		}
		if (!bone->scales.empty())
		{
			ds->type |= DIM_FLAG_SCALE_X;
			ds->type |= DIM_FLAG_SCALE_Y;
			ds->dims_count[DIM_IDX_SCALE_X] = ds->dims_count[DIM_IDX_SCALE_Y] = bone->scales.size();

			int max_frame = (int)(bone->scales.back().time * 30 + 0.5f);
			if (max_frame > m_max_frame) {
				m_max_frame = max_frame;
			}

			rg_frame* frame = &ds->frames[frame_ptr];
			for (int i = 0, n = bone->scales.size(); i < n; ++i) {
				frame[i].time = (int)(bone->scales[i].time * 30 + 0.5f);
				frame[i].lerp = 0;
				frame[i].data = bone->scales[i].scale.x;
			}
			frame_ptr += bone->scales.size();

			frame = &ds->frames[frame_ptr];
			for (int i = 0, n = bone->scales.size(); i < n; ++i) {
				frame[i].time = (int)(bone->scales[i].time * 30 + 0.5f);
				frame[i].lerp = 0;
				frame[i].data = bone->scales[i].scale.y;
			}
			frame_ptr += bone->scales.size();
		}
	
		m_sheets[i] = ds;
	}
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