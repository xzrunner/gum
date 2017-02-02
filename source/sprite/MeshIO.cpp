#include "MeshIO.h"
#include "JsonSerializer.h"
#include "SkeletonIO.h"

#include <SM_Vector.h>
#include <sprite2/MeshTransform2.h>
#include <sprite2/MeshJoint.h>
#include <sprite2/MeshSkeleton.h>
#include <sprite2/MeshTriangle.h>
#include <sprite2/Mesh.h>

#include <queue>

#include <assert.h>

namespace gum
{

/************************************************************************/
/* MeshTransform                                                        */
/************************************************************************/

void MeshIO::Load(const Json::Value& val, s2::MeshTransform2& trans, const s2::Mesh& mesh)
{
// 	std::map<sm::vec2, sm::vec2, sm::Vector2Cmp>& map = trans.GetMap();
// 	map.clear();
// 	std::vector<sm::vec2> from, to;
// 	gum::JsonSerializer::Load(val["trans"]["from"], from);
// 	gum::JsonSerializer::Load(val["trans"]["to"], to);
// 	for (int i = 0, n = from.size(); i < n; ++i) {
// 		map.insert(std::make_pair(from[i], to[i]));
// 	}

	//////////////////////////////////////////////////////////////////////////

	const std::vector<s2::MeshTriangle*>& tris = mesh.GetTriangles();
	std::multimap<sm::vec2, int, sm::Vector2Cmp> map2idx;
	int idx = 0;
	for (int i = 0, n = tris.size(); i < n; ++i) {
		s2::MeshTriangle* tri = tris[i];
		for (int j = 0; j < 3; ++j) {
			map2idx.insert(std::make_pair(tri->nodes[j]->ori_xy, idx++));
		}
	}

 	std::vector<sm::vec2> from, to;
 	gum::JsonSerializer::Load(val["trans"]["from"], from);
 	gum::JsonSerializer::Load(val["trans"]["to"], to);
	assert(from.size() == to.size());

	trans.Clear();

	for (int i = 0, n = from.size(); i < n; ++i) 
	{
		sm::vec2 pos = to[i] - from[i];
		std::multimap<sm::vec2, int, sm::Vector2Cmp>::const_iterator 
			itr_begin = map2idx.lower_bound(from[i]),
			itr_end = map2idx.upper_bound(from[i]);
		assert(itr_begin != map2idx.end());
		std::multimap<sm::vec2, int, sm::Vector2Cmp>::const_iterator itr = itr_begin;
		for ( ; itr != itr_end; ++itr) {
			trans.Add(itr->second, pos);
			break;	// todo
		}
	}
}

void MeshIO::Store(Json::Value& val, const s2::MeshTransform2& trans, const s2::Mesh& mesh)
{
// 	Json::Value& trans_val = val["trans"];
// 
// 	int count = 0;
// 	const std::map<sm::vec2, sm::vec2, sm::Vector2Cmp>& map = trans.GetMap();
// 	std::map<sm::vec2, sm::vec2, sm::Vector2Cmp>::const_iterator itr = map.begin();
// 	for ( ; itr != map.end(); ++itr) 
// 	{
// 		trans_val["from"][count * 2] = itr->first.x;
// 		trans_val["from"][count * 2 + 1] = itr->first.y;
// 		trans_val["to"][count * 2] = itr->second.x;
// 		trans_val["to"][count * 2 + 1] = itr->second.y;
// 		++count;
// 	}

	//////////////////////////////////////////////////////////////////////////

	Json::Value& trans_val = val["trans"];

	int count = 0;
	const std::vector<s2::MeshTriangle*>& tris = mesh.GetTriangles();
	const std::vector<std::pair<int, sm::vec2> >& map_trans = trans.GetTrans();
	for (int i = 0, n = map_trans.size(); i < n; ++i)
	{
		int idx = map_trans[i].first;
		int tri = idx / 3;
		int node = idx - tri * 3;
		const sm::vec2& from = tris[tri]->nodes[node]->ori_xy;
		sm::vec2 to = from + map_trans[i].second;
 		trans_val["from"][count * 2]     = from.x;
 		trans_val["from"][count * 2 + 1] = from.y;
 		trans_val["to"][count * 2]     = to.x;
 		trans_val["to"][count * 2 + 1] = to.y;
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