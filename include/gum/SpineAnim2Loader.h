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
struct rg_ds_joint;
struct rg_ds_skin;

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
	void CreateSlots(const SpineParser& parser);
	void CreateJoints();
	void CreateSkeleton();
	void InitRoot();
 	void InitPose(const SpineParser& parser);

	void LoadDopesheetsJoints(const SpineParser::Animation& anim);
	void LoadDopesheetsJoints(const SpineParser::AnimBone* bone, struct rg_ds_joint* ds);
	
	void LoadDopesheetsSkins(const SpineParser::Animation& anim);
	void LoadDopesheetsSkins(const SpineParser::AnimSlot* slot, struct rg_ds_skin* ds);

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

private:
	s2::Anim2Symbol* m_sym;

	const SymbolLoader* m_spr_loader;

	int m_num;

	// middle
	std::vector<JointData>     m_joints_data;
	std::vector<SlotData>      m_slots_data;
	std::map<std::string, int> m_bone2joint;
	std::map<std::string, int> m_map2skin;

	// dst
	std::vector<rg_skin> m_skins;
	std::vector<rg_slot> m_slots;
	int            m_joint_count;
	rg_joint**     m_joints;
	rg_skeleton*   m_sk;
	rg_joint*      m_root;
	rg_ds_joint**  m_ds_joints;
	rg_ds_skin**   m_ds_skins;
	int            m_max_frame;

}; // SpineAnim2Loader

}

#endif // _GUM_SPINE_ANIM2_LOADER_H_