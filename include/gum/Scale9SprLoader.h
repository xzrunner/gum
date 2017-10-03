#ifndef _GUM_SCALE9_SPR_LOADER_H_
#define _GUM_SCALE9_SPR_LOADER_H_

#include <cu/uncopyable.h>

#include <memory>

#include <json/json.h>

namespace s2 { class Scale9Sprite; }
namespace simp { class NodeScale9Spr; }

namespace gum
{

class Scale9SprLoader : private cu::Uncopyable
{
public:
	Scale9SprLoader(const std::shared_ptr<s2::Scale9Sprite>& spr);

	void LoadJson(const Json::Value& val, const std::string& dir);
	void LoadBin(const simp::NodeScale9Spr* node);

private:
	std::shared_ptr<s2::Scale9Sprite> m_spr;

}; // Scale9SprLoader

}

#endif // _GUM_SCALE9_SPR_LOADER_H_
