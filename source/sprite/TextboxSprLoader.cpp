#include "gum/TextboxSprLoader.h"
#include "gum/TextboxLoader.h"

#include <sprite2/TextboxSprite.h>
#include <sprite2/UpdateParams.h>
#include <simp/NodeLabel.h>

namespace gum
{

TextboxSprLoader::TextboxSprLoader(const std::shared_ptr<s2::TextboxSprite>& spr)
	: m_spr(spr)
{
}

void TextboxSprLoader::LoadJson(const Json::Value& val)
{
	if (!m_spr) {
		return;
	}

	const Json::Value& text_val = val["text"];

	TextboxLoader loader(m_spr->GetTextbox());
	loader.LoadJson(text_val);

	m_spr->SetText(s2::UpdateParams(), text_val["text"].asString().c_str());
}

void TextboxSprLoader::LoadBin(const simp::NodeLabel* node)
{
	if (!m_spr) {
		return;
	}

	TextboxLoader loader(m_spr->GetTextbox());
	loader.LoadBin(node);
	
	if (node->text) {
		m_spr->SetText(s2::UpdateParams(), node->text);
	}
}

}