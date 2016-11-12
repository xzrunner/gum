#ifndef _GUM_SPINE_ANIM2_LOADER_H_
#define _GUM_SPINE_ANIM2_LOADER_H_

#include <CU_Uncopyable.h>
#include <rigging/rg_joint_pose.h>

#include <json/value.h>

namespace s2 { class Anim2Symbol; class Symbol; }

struct rg_joint;
struct rg_skeleton;
struct rg_dopesheet;

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

	void LoadSprites(const SpineParser& parser, const std::string& img_dir);

	void LoadJointsData(const SpineParser& parser);
	void ConnectJoints(const SpineParser& parser);

	void CreateJoints();
	void CreateSkeleton();
	void InitRoot();
 	void InitPose(const SpineParser& parser);

	void LoadDopesheets(const SpineParser& parser);

private:
	struct JointData
	{
		std::string name;

		s2::Symbol* skin;
		rg_joint_pose pose;
		std::vector<int> children;

		JointData();
	};

private:
	s2::Anim2Symbol* m_sym;

	const SymbolLoader* m_spr_loader;

	int m_num;
	std::vector<s2::Symbol*> m_syms;

	// middle
	std::vector<JointData>     m_joints_data;
	std::map<std::string, int> m_map2jointdata;

	// dst
	int            m_joint_count;
	rg_joint**     m_joints;
	rg_skeleton*   m_sk;
	rg_joint*      m_root;
	rg_dopesheet** m_sheets;
	int            m_max_frame;

}; // SpineAnim2Loader

}

#endif // _GUM_SPINE_ANIM2_LOADER_H_