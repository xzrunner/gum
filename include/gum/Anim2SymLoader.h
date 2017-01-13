#ifndef _GUM_ANIM2_SYM_LOADER_H_
#define _GUM_ANIM2_SYM_LOADER_H_

#include <CU_Uncopyable.h>
#include <SM_Vector.h>
#include <simp/NodeAnim2.h>

#include <json/json.h>

#include <string>

namespace s2 { class Anim2Symbol; }

struct rg_pose_srt;
struct rg_skeleton;

namespace gum
{

class SymbolLoader;
class JointLoader;

class Anim2SymLoader : private cu::Uncopyable
{
public:
	Anim2SymLoader(s2::Anim2Symbol* sym, const SymbolLoader* sym_loader = NULL);
	~Anim2SymLoader();

	void LoadJson(const std::string& filepath);
	void LoadBin(const simp::NodeAnim2* node);

private:
	static int CalcNodeSize(const simp::NodeAnim2* node);

	static void LoadBinSRT(rg_pose_srt& dst, const simp::NodeAnim2::Srt& src);

	static void InitJointChildren(rg_skeleton* sk);

private:
	s2::Anim2Symbol* m_sym;

	const SymbolLoader* m_sym_loader;

}; // Anim2SymLoader

}

#endif // _GUM_ANIM2_SYM_LOADER_H_