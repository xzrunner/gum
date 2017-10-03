#ifndef _GUM_TEXTBOX_SPR_LOADER_H_
#define _GUM_TEXTBOX_SPR_LOADER_H_

#include <cu/uncopyable.h>

#include <json/json.h>

#include <memory>

namespace s2 { class TextboxSprite; }
namespace simp { class NodeLabel; }

namespace gum
{

class TextboxSprLoader : private cu::Uncopyable
{
public:
	TextboxSprLoader(const std::shared_ptr<s2::TextboxSprite>& spr);

	void LoadJson(const Json::Value& val);
	void LoadBin(const simp::NodeLabel* node);

private:
	std::shared_ptr<s2::TextboxSprite> m_spr;

}; // TextboxSprLoader

}

#endif // _GUM_TEXTBOX_SPR_LOADER_H_