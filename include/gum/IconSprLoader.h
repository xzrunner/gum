#ifndef _GUM_ICON_SPR_LOADER_H_
#define _GUM_ICON_SPR_LOADER_H_

#include <cu/uncopyable.h>

#include <json/json.h>

#include <memory>

namespace s2 { class IconSprite; }
namespace simp { class NodeIconSpr; }

namespace gum
{

class IconSprLoader : private cu::Uncopyable
{
public:
	IconSprLoader(const std::shared_ptr<s2::IconSprite>& spr);

	void LoadJson(const Json::Value& val, const std::string& dir);
	void LoadBin(const simp::NodeIconSpr* node);

private:
	std::shared_ptr<s2::IconSprite> m_spr;

}; // IconSprLoader

}

#endif // _GUM_ICON_SPR_LOADER_H_
