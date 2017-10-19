#ifndef _GUM_ANIM2_SYM_LOADER_H_
#define _GUM_ANIM2_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <SM_Vector.h>
#include <cu/cu_stl.h>

#include <json/json.h>

#include <memory>

namespace s2 { class Anim2Symbol; }
namespace simp { class NodeAnim2; }

struct rg_pose_srt;
struct rg_skeleton;

namespace gum
{

class SymbolLoader;
class JointLoader;

class Anim2SymLoader : private cu::Uncopyable
{
public:
	Anim2SymLoader(const std::shared_ptr<s2::Anim2Symbol>& sym, 
		const std::shared_ptr<const SymbolLoader>& sym_loader = NULL);

	void LoadJson(const CU_STR& filepath);
	void LoadBin(const simp::NodeAnim2* node);

private:
	static int CalcNodeSize(const simp::NodeAnim2* node);

	static void InitJointChildren(rg_skeleton* sk);

private:
	std::shared_ptr<s2::Anim2Symbol> m_sym;

	std::shared_ptr<const SymbolLoader> m_sym_loader;

}; // Anim2SymLoader

}

#endif // _GUM_ANIM2_SYM_LOADER_H_