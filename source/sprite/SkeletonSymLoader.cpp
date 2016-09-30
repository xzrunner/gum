#include "SkeletonSymLoader.h"
#include "FilepathHelper.h"
#include "SpriteFactory.h"
#include "JointCoordsIO.h"

#include <sprite2/SkeletonSymbol.h>
#include <sprite2/Joint.h>
#include <sprite2/Skeleton.h>
#include <sprite2/S2_Sprite.h>

#include <fstream>

#include <assert.h>

namespace gum
{

SkeletonSymLoader::SkeletonSymLoader(s2::SkeletonSymbol* sym)
	: m_sym(sym)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

SkeletonSymLoader::~SkeletonSymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

struct Joint
{
	s2::LocalPose local;
	s2::WorldPose world;
	sm::vec2 skin;
	int parent;
	int idx;

	Joint() : parent(-1), idx(-1) {}
};

void SkeletonSymLoader::LoadJson(const std::string& filepath)
{
	if (!m_sym) {
		return;
	}

	Json::Value val;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, val);
	fin.close();

	int num = val["skeleton"].size();
	assert(num == val["sprite"].size());

	if (num == 0) {
		return;
	}

	// sprites
	std::vector<s2::Sprite*> sprs;
	sprs.reserve(num);
	std::string dir = FilepathHelper::Dir(filepath);
	for (int i = 0; i < num; ++i) {
		sprs.push_back(SpriteFactory::Instance()->Create(val["sprite"][i], dir));		
	}

	// joints
	std::vector<s2::Joint*> joints;
	joints.reserve(num);
	std::vector<Joint> src_joints;
	for (int i = 0; i < num; ++i) 
	{
		s2::Sprite* spr = sprs[i];
		const Json::Value& joints_val = val["skeleton"][i]["joint"];
		s2::Joint* dst_joint = NULL;
		for (int j = 0, m = joints_val.size(); j < m; ++j) 
		{
			const Json::Value& joint_val = joints_val[j];
			Joint src_joint;
			JointCoordsIO::Load(joint_val["local_pose"], src_joint.local);
			JointCoordsIO::Load(joint_val["world_pose"], src_joint.world);
			JointCoordsIO::Load(joint_val["skin_pose"], src_joint.skin);
			src_joint.idx = i;
			if (joint_val.isMember("parent")) {
				src_joint.parent = joint_val["parent"].asInt();
				if (!dst_joint) {
					dst_joint = new s2::Joint(spr, -src_joint.skin);
					dst_joint->SetWorldPose(src_joint.world);
					dst_joint->SetLocalPose(src_joint.local);
				}
			}
			src_joints.push_back(src_joint);
		}
		if (!dst_joint) {
			dst_joint = new s2::Joint(spr, sm::vec2(0, 0));
			dst_joint->SetWorldPose(s2::WorldPose(spr->GetCenter(), spr->GetAngle()));
			dst_joint->SetLocalPose(s2::LocalPose(0, 0));
		}
		joints.push_back(dst_joint);
	}

	// connect
	for (int i = 0, n = src_joints.size(); i < n; ++i) {
		Joint src_joint = src_joints[i];
		if (src_joint.parent != -1) {
			joints[src_joints[src_joint.parent].idx]->Connect(joints[src_joint.idx]);
		}
	}

	// root
	assert(!joints.empty());
	const s2::Joint* root = joints[0];
	while (const s2::Joint* parent = root->GetParent()) {
		root = parent;
	}

	// output
	m_sym->SetSkeleton(new s2::Skeleton(root, joints));
	for_each(joints.begin(), joints.end(), cu::RemoveRefFonctor<s2::Joint>());
}

}