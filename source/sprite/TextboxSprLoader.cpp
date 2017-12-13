#include "gum/TextboxSprLoader.h"
#include "gum/TextboxLoader.h"
#include "gum/Config.h"

#include <sprite2/TextboxSprite.h>
#include <sprite2/TextTable.h>
#include <sprite2/UpdateParams.h>
#include <simp/NodeLabel.h>

namespace gum
{

TextboxSprLoader::TextboxSprLoader(s2::TextboxSprite& spr)
	: m_spr(spr)
{
}

void TextboxSprLoader::LoadJson(const Json::Value& val)
{
	const Json::Value& text_val = val["text"];

	TextboxLoader loader(m_spr.GetTextbox());
	loader.LoadJson(text_val);

	CU_STR text = text_val["text"].asString().c_str();
	if (text.empty()) {
		text = s2::TextTable::Instance()->Query(
			Config::Instance()->GetLanguage(), text_val["tid"].asString().c_str());
	}
	m_spr.SetText(s2::UpdateParams(), text);

	CU_STR tid = text_val["text"].asString().c_str();
	m_spr.SetTID(tid);
}

void TextboxSprLoader::LoadBin(const simp::NodeLabel* node)
{
	TextboxLoader loader(m_spr.GetTextbox());
	loader.LoadBin(node);
	
	if (node->text) {
		m_spr.SetText(s2::UpdateParams(), node->text);
	} else if (node->tid) {
		auto text = s2::TextTable::Instance()->Query(
			Config::Instance()->GetLanguage(), node->tid);
		m_spr.SetText(s2::UpdateParams(), text);
	}
}

}