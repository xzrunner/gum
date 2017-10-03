#ifndef _GUM_ANIM2_SPR_LOADER_H_
#define _GUM_ANIM2_SPR_LOADER_H_

#include <cu/uncopyable.h>

#include <json/json.h>

#include <memory>

namespace s2 { class Anim2Sprite; }
namespace simp { class NodeAnim2Spr; }

namespace gum
{

class Anim2SprLoader : private cu::Uncopyable
{
public:
	Anim2SprLoader(const std::shared_ptr<s2::Anim2Sprite>& spr);

	void LoadJson(const Json::Value& val, const std::string& dir);
	void LoadBin(const simp::NodeAnim2Spr* node);

private:
	std::shared_ptr<s2::Anim2Sprite> m_spr;

}; // Anim2SprLoader

}

#endif // _GUM_ANIM2_SPR_LOADER_H_
