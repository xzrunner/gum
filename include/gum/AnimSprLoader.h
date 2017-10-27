#ifndef _GUM_ANIM_SPR_LOADER_H_
#define _GUM_ANIM_SPR_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <json/json.h>

namespace s2 { class AnimSprite; }
namespace simp { class NodeAnimationSpr; }

namespace gum
{

class AnimSprLoader : private cu::Uncopyable
{
public:
	AnimSprLoader(s2::AnimSprite& spr);

	void LoadJson(const Json::Value& val, const CU_STR& dir);
	void LoadBin(const simp::NodeAnimationSpr* node);

private:
	s2::AnimSprite& m_spr;

}; // AnimSprLoader

}

#endif // _GUM_ANIM_SPR_LOADER_H_
