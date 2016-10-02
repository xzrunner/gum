#include "MeshIO.h"
#include "JsonSerializer.h"
#include "SkeletonIO.h"

#include <SM_Vector.h>
#include <sprite2/MeshTransform.h>
#include <sprite2/MeshJoint.h>
#include <sprite2/MeshSkeleton.h>

#include <queue>

#include <assert.h>

namespace gum
{

/************************************************************************/
/* MeshTransform                                                        */
/************************************************************************/

void MeshIO::Load(const Json::Value& val, s2::MeshTransform& trans)
{
	std::map<sm::vec2, sm::vec2, sm::Vector2Cmp>& map = trans.GetMap();
	map.clear();
	std::vector<sm::vec2> from, to;
	gum::JsonSerializer::Load(val["trans"]["from"], from);
	gum::JsonSerializer::Load(val["trans"]["to"], to);
	for (int i = 0, n = from.size(); i < n; ++i) {
		map.insert(std::make_pair(from[i], to[i]));
	}
}

void MeshIO::Store(Json::Value& val, const s2::MeshTransform& trans)
{
	Json::Value& trans_val = val["trans"];

	int count = 0;
	const std::map<sm::vec2, sm::vec2, sm::Vector2Cmp>& map = trans.GetMap();
	std::map<sm::vec2, sm::vec2, sm::Vector2Cmp>::const_iterator itr = map.begin();
	for ( ; itr != map.end(); ++itr) 
	{
		trans_val["from"][count * 2] = itr->first.x;
		trans_val["from"][count * 2 + 1] = itr->first.y;
		trans_val["to"][count * 2] = itr->second.x;
		trans_val["to"][count * 2 + 1] = itr->second.y;
		++count;
	}
}

/************************************************************************/
/* MeshSkeleton                                                         */
/************************************************************************/

void MeshIO::Load(const Json::Value& val, s2::MeshSkeleton& sk)
{
	struct Joint
	{
		s2::JointPose world;
		int parent;
	};

	std::vector<Joint> joints;
	joints.reserve(val["joint"].size());
	for (int i = 0, n = val["joint"].size(); i < n; ++i)
	{
		const Json::Value& src = val["joint"][i];
		Joint dst;
		SkeletonIO::Load(src["world_pose"], dst.world);
		dst.parent = src["parent"].asInt();
		joints.push_back(dst);
	}

	std::vector<s2::MeshJoint*> mjoints;
	mjoints.reserve(joints.size());
	s2::MeshJoint* root = NULL;
	for (int i = 0, n = joints.size(); i < n; ++i)
	{
		const Joint& src = joints[i];
		if (src.parent == -1) {
			mjoints.push_back(root = new s2::MeshJoint(NULL, src.world));
		} else {
			assert(src.parent < mjoints.size());
			mjoints.push_back(new s2::MeshJoint(mjoints[src.parent], src.world));
		}
	}

	if (root) {
		sk.SetRoot(root);
	}
}

void MeshIO::Store(Json::Value& val, const s2::MeshSkeleton& trans)
{
	const s2::MeshJoint* root = trans.GetRoot();
	if (!root) {
		return;
	}

	std::vector<const s2::MeshJoint*> joints;
	std::queue<const s2::MeshJoint*> buf;
	buf.push(root);
	while (!buf.empty()) {
		const s2::MeshJoint* joint = buf.front(); buf.pop();
		joints.push_back(joint);
		const std::vector<s2::MeshJoint*>& children = joint->GetChildren();
		for (int i = 0, n = children.size(); i < n; ++i) {
			buf.push(children[i]);
		}
	}

	for (int i = 0, n = joints.size(); i < n; ++i) 
	{
		const s2::MeshJoint* joint = joints[i];
		Json::Value cval;
		SkeletonIO::Store(cval["world_pose"], joint->GetWorldPose());
		int parent = -1;
		for (int idx = 0; idx < n; ++idx) {
			if (joints[idx] == joint->GetParent()) {
				parent = idx;
				break;
			}
		}
		cval["parent"] = parent;
		val["joint"][i] = cval;
	}
}

}