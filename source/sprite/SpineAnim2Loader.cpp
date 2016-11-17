#include "SpineAnim2Loader.h"
#include "SymbolLoader.h"
#include "SpineParser.h"
#include "FilepathHelper.h"

#include <sprite2/Anim2Symbol.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/MeshSymbol.h>
#include <sprite2/TrianglesMesh.h>
#include <rigging/rg_joint.h>
#include <rigging/rg_skeleton.h>
#include <rigging/rg_timeline.h>
#include <rigging/rg_animation.h>

#include <assert.h>

namespace gum
{

static const int ANIM_IDX = 0;

SpineAnim2Loader::SpineAnim2Loader(s2::Anim2Symbol* sym, 
								   const SymbolLoader* sym_loader)
	: m_sym(sym)
	, m_sym_loader(sym_loader)
	, m_num(0)
	, m_joint_count(0)
	, m_joints(NULL)
	, m_sk(NULL)
	, m_root(NULL)
	, m_tl_joints(NULL)
	, m_tl_skins(NULL)
	, m_max_frame(0)
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

SpineAnim2Loader::~SpineAnim2Loader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_sym_loader->RemoveReference();
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

	LoadJointsData(parser);
	ConnectJoints(parser);

	CreateSkins(parser, img_dir);
	CreateSlots(parser);
	CreateJoints();
	CreateSkeleton();

	InitRoot();
	InitPose(parser);

	LoadTimelineJoints(parser.anims[ANIM_IDX]);
	LoadTimelineSkins(parser.anims[ANIM_IDX]);

	rg_animation* anim = (rg_animation*)malloc(SIZEOF_RG_ANIM);
	anim->sk = m_sk;

	s2::Symbol* sym = static_cast<s2::Symbol*>(anim->sk->skins[10].ud);

	anim->timeline.joints = m_tl_joints;
	anim->timeline.skins = m_tl_skins;
	anim->max_frame = m_max_frame;
	m_sym->SetAnim(anim);
}

void SpineAnim2Loader::Clear()
{
	m_num = 0;

//	m_root = NULL;
}

void SpineAnim2Loader::LoadJointsData(const SpineParser& parser)
{
	std::map<std::string, SpineParser::Bone>::const_iterator itr 
		= parser.bones.begin();
	for ( ; itr != parser.bones.end(); ++itr)
	{
		const SpineParser::Bone& bone = itr->second;
		m_bone2joint.insert(std::make_pair(bone.name, m_joints_data.size()));
		m_joints_data.push_back(JointData(bone.name));
	}
}

//void SpineAnim2Loader::LoadJointsData(const SpineParser& parser)
//{
//	for (int i = 0, n = parser.slots.size(); i < n; ++i)
//	{
//		const SpineParser::Slot& slot = parser.slots[i];
//		const SpineParser::SkinItem* item = parser.QuerySkin(slot);
//		if (!item) {
//			continue;
//		}
//
//		m_bone2joint.insert(std::make_pair(slot.bone, m_joints_data.size()));
//		m_joints_data.push_back(JointData(slot.name, slot.bone, slot.attachment));
//	}
//
//	std::map<std::string, SpineParser::Bone>::const_iterator itr 
//		= parser.bones.begin();
//	for ( ; itr != parser.bones.end(); ++itr)
//	{
//		const SpineParser::Bone& bone = itr->second;
//		std::map<std::string, int>::iterator itr_joint 
//			= m_bone2joint.find(bone.name);
//		if (itr_joint != m_bone2joint.end()) {
//			continue;
//		}
//
//		m_bone2joint.insert(std::make_pair(bone.name, m_joints_data.size()));
//		m_joints_data.push_back(JointData(bone.name, bone.name, ""));
//	}
//}

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
			= m_bone2joint.find(bone.name);
		assert(itr_joint != m_bone2joint.end());
		int child = itr_joint->second;
		itr_joint = m_bone2joint.find(bone.parent);
		assert(itr_joint != m_bone2joint.end());
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

void SpineAnim2Loader::CreateSkins(const SpineParser& parser, const std::string& img_dir)
{
	int skin_count = 0;
	for (int i = 0, n = parser.skins.size(); i < n; ++i) {
		skin_count += parser.skins[i].items.size();
	}

	m_skins.reserve(skin_count);
	int ptr = 0;
	for (int i = 0, n = parser.skins.size(); i < n; ++i) 
	{
		for (int j = 0, m = parser.skins[i].items.size(); j < m; ++j) 
		{
			const SpineParser::SkinItem& src = parser.skins[i].items[j];
			if (src.type != SpineParser::SKIN_IMAGE && src.type != SpineParser::SKIN_MESH) {
				continue;
			}

			m_map2skin.insert(std::make_pair(src.name, ptr++));

			rg_skin skin;
			skin.ud = NULL;
			switch (src.type)
			{
			case SpineParser::SKIN_IMAGE:
				CreateImageSkin(skin, src, img_dir);
				break;
			case SpineParser::SKIN_MESH:
				CreateMeshSkin(skin, src, img_dir);
				break;
			}
			m_skins.push_back(skin);
		}
	}
}

void SpineAnim2Loader::CreateImageSkin(rg_skin& dst, const SpineParser::SkinItem& src, const std::string& img_dir) const
{
	dst.local.trans[0] = src.pos.x;
	dst.local.trans[1] = src.pos.y;
	dst.local.rot      = src.angle;
	dst.local.scale[0] = 1;
	dst.local.scale[1] = 1;
	std::string filepath = FilepathHelper::Absolute(img_dir, src.path + ".png");
	dst.ud = m_sym_loader->Create(filepath);
}

void SpineAnim2Loader::CreateMeshSkin(rg_skin& dst, const SpineParser::SkinItem& src, const std::string& img_dir) const
{
	rg_pose_srt_identity(&dst.local);

	s2::MeshSymbol* sym = new s2::MeshSymbol;

	std::string filepath = FilepathHelper::Absolute(img_dir, src.path + ".png");
	s2::Symbol* base_sym = m_sym_loader->Create(filepath);

	s2::TrianglesMesh* mesh = new s2::TrianglesMesh(base_sym);
	mesh->SetData(src.vertices, src.texcoords, src.triangles);

	sym->SetMesh(mesh);
	mesh->RemoveReference();

	dst.ud = (s2::Symbol*)(sym);
}

void SpineAnim2Loader::CreateSlots(const SpineParser& parser)
{
	m_slots.reserve(parser.slots.size());
	for (int i = 0, n = parser.slots.size(); i < n; ++i)
	{
		const SpineParser::Slot& src = parser.slots[i];
		rg_slot dst;

		std::map<std::string, int>::iterator itr_joint = m_bone2joint.find(src.bone);
		assert(itr_joint != m_bone2joint.end());
		dst.joint = itr_joint->second;

		dst.skin = RG_SKIN_UNKNOWN;
		std::map<std::string, int>::iterator itr_skin = m_map2skin.find(src.attachment);
		if (itr_skin != m_map2skin.end()) {
			dst.skin = itr_skin->second;
		}

		m_slots_data.push_back(src.name);

		m_slots.push_back(dst);
	}
}

void SpineAnim2Loader::CreateJoints()
{
	m_joint_count = m_joints_data.size();
	m_joints = (rg_joint**)malloc(sizeof(rg_joint*) * m_joint_count);
	for (int i = 0, n = m_joint_count; i < n; ++i) 
	{
		const JointData& src = m_joints_data[i];
		rg_joint* dst = (rg_joint*)malloc(SIZEOF_RG_JOINT + sizeof(uint8_t) * src.children.size());
		dst->name           = strdup(src.name.c_str());
		dst->parent         = 0xff;
		dst->children_count = src.children.size();
		for (int j = 0; j < dst->children_count; ++j) {
			dst->children[j] = src.children[j];
		}
		m_joints[i] = dst;
	}
	for (int i = 0; i < m_joint_count; ++i) {
		const JointData& src = m_joints_data[i];
		for (int j = 0, m = src.children.size(); j < m; ++j) {
			m_joints[src.children[j]]->parent = i;
		}
	}
}

void SpineAnim2Loader::CreateSkeleton()
{
	int skins_sz = SIZEOF_RG_SKIN * m_skins.size();
	int slots_sz = SIZEOF_RG_SLOT * m_slots.size();
	m_sk = (rg_skeleton*)malloc(SIZEOF_RG_SKELETON + skins_sz + slots_sz);

	m_sk->joints = m_joints;
	m_sk->joint_count = m_joint_count;
	m_sk->root = -1;

	m_sk->skin_count = m_skins.size();
	for (int i = 0; i < m_sk->skin_count; ++i) {
		m_sk->skins[i] = m_skins[i];
	}

	m_sk->slot_count = m_slots.size();
	m_sk->slots = (rg_slot*)((intptr_t)(m_sk + 1) + skins_sz);
	for (int i = 0; i < m_sk->slot_count; ++i) {
		m_sk->slots[i] = m_slots[i];
	}
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

		rg_pose_mat_identity(&dst->world_pose);
	}

	rg_pose_mat_build(&m_root->world_pose, &m_root->local_pose);
	rg_joint_update(m_root, m_sk);
}

void SpineAnim2Loader::LoadTimelineJoints(const SpineParser::Animation& anim)
{
	m_tl_joints = (rg_tl_joint**)malloc(sizeof(struct rg_tl_joint*) * m_sk->joint_count);
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
		if (!bone) {
			m_tl_joints[i] = NULL;
			continue;
		}

		int frame_count = bone->translates.size() * 2 + bone->rotates.size() + bone->scales.size() * 2;
		int sz = SIZEOF_RG_TIMELINE_JOINT + sizeof(struct rg_joint_sample) * frame_count;
		struct rg_tl_joint* joint = (struct rg_tl_joint*)malloc(sz);
		memset(joint, 0, sz);

		LoadTimelineJoints(bone, joint);
	
		m_tl_joints[i] = joint;
	}
}

void SpineAnim2Loader::LoadTimelineJoints(const SpineParser::AnimBone* bone, struct rg_tl_joint* joint)
{
	int frame_ptr = 0;
	if (!bone->translates.empty())
	{
		joint->type |= DIM_FLAG_TRANS_X;
		joint->type |= DIM_FLAG_TRANS_Y;
		joint->dims_count[DIM_IDX_TRANS_X] = joint->dims_count[DIM_IDX_TRANS_Y] = bone->translates.size();

		int max_frame = (int)(bone->translates.back().time * 30 + 0.5f);
		if (max_frame > m_max_frame) {
			m_max_frame = max_frame;
		}

		rg_joint_sample* frame = &joint->samples[frame_ptr];
		for (int i = 0, n = bone->translates.size(); i < n; ++i) {
			frame[i].time = (int)(bone->translates[i].time * 30 + 0.5f);
			frame[i].lerp = 0;
			frame[i].data = bone->translates[i].trans.x;
		}
		frame_ptr += bone->translates.size();

		frame = &joint->samples[frame_ptr];
		for (int i = 0, n = bone->translates.size(); i < n; ++i) {
			frame[i].time = (int)(bone->translates[i].time * 30 + 0.5f);
			frame[i].lerp = 0;
			frame[i].data = bone->translates[i].trans.y;
		}
		frame_ptr += bone->translates.size();
	}
	if (!bone->rotates.empty())
	{
		joint->type |= DIM_FLAG_ROT;
		joint->dims_count[DIM_IDX_ROT] = bone->rotates.size();

		int max_frame = (int)(bone->rotates.back().time * 30 + 0.5f);
		if (max_frame > m_max_frame) {
			m_max_frame = max_frame;
		}

		rg_joint_sample* frame = &joint->samples[frame_ptr];
		for (int i = 0, n = bone->rotates.size(); i < n; ++i) {
			frame[i].time = (int)(bone->rotates[i].time * 30 + 0.5f);
			frame[i].lerp = 0;
			frame[i].data = bone->rotates[i].rot;
		}
		frame_ptr += bone->rotates.size();
	}
	if (!bone->scales.empty())
	{
		joint->type |= DIM_FLAG_SCALE_X;
		joint->type |= DIM_FLAG_SCALE_Y;
		joint->dims_count[DIM_IDX_SCALE_X] = joint->dims_count[DIM_IDX_SCALE_Y] = bone->scales.size();

		int max_frame = (int)(bone->scales.back().time * 30 + 0.5f);
		if (max_frame > m_max_frame) {
			m_max_frame = max_frame;
		}

		rg_joint_sample* frame = &joint->samples[frame_ptr];
		for (int i = 0, n = bone->scales.size(); i < n; ++i) {
			frame[i].time = (int)(bone->scales[i].time * 30 + 0.5f);
			frame[i].lerp = 0;
			frame[i].data = bone->scales[i].scale.x;
		}
		frame_ptr += bone->scales.size();

		frame = &joint->samples[frame_ptr];
		for (int i = 0, n = bone->scales.size(); i < n; ++i) {
			frame[i].time = (int)(bone->scales[i].time * 30 + 0.5f);
			frame[i].lerp = 0;
			frame[i].data = bone->scales[i].scale.y;
		}
		frame_ptr += bone->scales.size();
	}
}

void SpineAnim2Loader::LoadTimelineSkins(const SpineParser::Animation& anim) 
{
	m_tl_skins = (rg_tl_skin**)malloc(sizeof(struct rg_tl_skin*) * m_sk->slot_count);
	for (int i = 0; i < m_sk->slot_count; ++i)
	{
		const std::string& name = m_slots_data[i].name;
		const SpineParser::AnimSlot* slot = NULL;
		for (int j = 0, m = anim.slots.size(); j < m; ++j) {
			if (name == anim.slots[j].name) {
				slot = &anim.slots[j];
				break;
			}
		}
		if (!slot) {
			m_tl_skins[i] = NULL;
			continue;
		}

		int skin_count = slot->skins.size();
		int sz = SIZEOF_RG_TIMELINE_SKIN + sizeof(struct rg_skin_sample) * skin_count;
		struct rg_tl_skin* skin = (struct rg_tl_skin*)malloc(sz);
		memset(skin, 0, sz);

		LoadTimelineSkins(slot, skin);

		m_tl_skins[i] = skin;
	}
}

void SpineAnim2Loader::LoadTimelineSkins(const SpineParser::AnimSlot* slot, struct rg_tl_skin* skin)
{
	skin->skin_count = slot->skins.size();
	for (int i = 0, n = slot->skins.size(); i < n; ++i) {
		skin->skins[i].time = (int)(slot->skins[i].first * 30 + 0.5f);
		if (slot->skins[i].second.empty()) {
			skin->skins[i].skin = RG_SKIN_NULL;
		} else {
			std::map<std::string, int>::iterator itr 
				= m_map2skin.find(slot->skins[i].second);
			assert(itr != m_map2skin.end());
			skin->skins[i].skin = itr->second;
		}
	}
}

}