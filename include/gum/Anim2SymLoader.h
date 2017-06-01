#ifndef _GUM_ANIM2_SYM_LOADER_H_
#define _GUM_ANIM2_SYM_LOADER_H_

#include <CU_Uncopyable.h>
#include <SM_Vector.h>

#include <json/json.h>

#include <string>

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
	Anim2SymLoader(s2::Anim2Symbol* sym, bool flatten, const SymbolLoader* sym_loader = NULL);
	~Anim2SymLoader();

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeAnim2* node);

private:
	static int CalcNodeSize(const simp::NodeAnim2* node);

	static void InitJointChildren(rg_skeleton* sk);

private:
	s2::Anim2Symbol* m_sym;

	bool m_flatten;

	const SymbolLoader* m_sym_loader;

}; // Anim2SymLoader

}

#endif // _GUM_ANIM2_SYM_LOADER_H_