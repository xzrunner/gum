#ifndef _GUM_SPINE_ANIM2_LOADER_H_
#define _GUM_SPINE_ANIM2_LOADER_H_

#include "SpineParser.h"

#include <CU_Uncopyable.h>
#include <rigging/rg_pose_srt.h>
#include <rigging/rg_skin.h>
#include <rigging/rg_slot.h>

#include <json/value.h>

namespace s2 { class Anim2Symbol; class Symbol; }

struct rg_joint;
struct rg_skeleton;
struct rg_tl_joint;
struct rg_tl_skin;
struct rg_tl_deform;

namespace gum
{

class SymbolLoader;
class SpineParser;

class SpineAnim2Loader : private cu::Uncopyable
{
public:
	SpineAnim2Loader(s2::Anim2Symbol* sym, const SymbolLoader* sym_loader = NULL);
	~SpineAnim2Loader();

	void LoadJson(const Json::Value& val, const std::string& dir);

	void LoadParser(const SpineParser& parser, const std::string& dir);

private:
	void Clear();

	void LoadJointsData(const SpineParser& parser);
	void ConnectJoints(const SpineParser& parser);

	void CreateSkins(const SpineParser& parser, const std::string& img_dir);
	void CreateImageSkin(rg_skin& dst, const SpineParser::SkinItem& src, const std::string& img_dir) const;
	void CreateMeshSkin(rg_skin& dst, const SpineParser::SkinItem& src, const std::string& img_dir) const;
	void CreateSlots(const SpineParser& parser);
	void CreateJoints();
	void CreateSkeleton();
	void InitRoot();
 	void InitPose(const SpineParser& parser);

	void LoadTimelineJoints(const SpineParser::Animation& anim);
	void LoadTimelineJoints(const SpineParser::AnimBone* bone, struct rg_tl_joint* joint);
	
	void LoadTimelineSkins(const SpineParser::Animation& anim);
	void LoadTimelineSkins(const SpineParser::AnimSlot* slot, struct rg_tl_skin* skin);

	void LoadTimelineDeforms(const SpineParser::Animation& anim);
	void LoadTimelineDeforms(const SpineParser::AnimDeform* deform);

private:
	struct JointData
	{
		std::string name;

		std::vector<int> children;

		JointData(const std::string& name) 
			: name(name) {}
	};

	struct SlotData
	{
		std::string name;

		SlotData(const std::string& name) 
			: name(name) {}
	};

	struct SkinData
	{
		std::string slot;
		std::string name;

		SkinData(const std::string& slot, const std::string& name)
			: slot(slot), name(name) {}
	};

private:
	s2::Anim2Symbol* m_sym;

	const SymbolLoader* m_sym_loader;

	int m_num;

	// middle
	std::vector<JointData>     m_joints_data;
	std::vector<SlotData>      m_slots_data;
	std::vector<SkinData>      m_skins_data;
	std::map<std::string, int> m_bone2joint;
	std::map<std::string, int> m_map2skin;

	// dst
	std::vector<rg_skin> m_skins;
	std::vector<rg_slot> m_slots;
	int            m_joint_count;
	rg_joint**     m_joints;
	rg_skeleton*   m_sk;
	rg_joint*      m_root;
	rg_tl_joint**  m_tl_joints;
	rg_tl_skin**   m_tl_skins;
	rg_tl_deform** m_tl_deforms;
	int            m_max_frame;

}; // SpineAnim2Loader

}

#endif // _GUM_SPINE_ANIM2_LOADER_H_