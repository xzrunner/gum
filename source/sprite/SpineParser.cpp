#include "SpineParser.h"

#include <fstream>

#include <assert.h>

namespace gum
{

void SpineParser::Parse(const std::string& filepath)
{
	Json::Value val;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, val);
	fin.close();

	Clear();

	ParseHeader(val["skeleton"]);
	ParseBones(val["bones"]);
	ParseSlots(val["slots"]);
	ParseSkins(val["skins"]["default"]);
}

const SpineParser::SkinItem* SpineParser::QuerySkin(const Slot& slot) const
{
	std::map<std::string, Skin>::const_iterator itr = m_skins.find(slot.name);
	assert(itr != m_skins.end());
	const Skin& skin = itr->second;
	for (int i = 0, n = skin.items.size(); i < n; ++i) {
		const SkinItem& item = skin.items[i];
		if (slot.attachment == item.name && item.type != "boundingbox") {
			return &item;
		}
	}
	return NULL;
}

void SpineParser::Clear()
{
	m_bones.clear();
	m_slots.clear();
	m_skins.clear();
}

void SpineParser::ParseHeader(const Json::Value& val)
{
	m_img_dir = "." + val["images"].asString();
}

void SpineParser::ParseBones(const Json::Value& val)
{
	for (int i = 0, n = val.size(); i < n; ++i)
	{
		const Json::Value& src = val[i];
		Bone dst;
		dst.name = src["name"].asString();
		dst.parent = src["parent"].asString();
		dst.pos.x = src["x"].asDouble();
		dst.pos.y = src["y"].asDouble();
		dst.angle = src["rotation"].asDouble();
		m_bones.insert(std::make_pair(dst.name, dst));
	}
}

void SpineParser::ParseSlots(const Json::Value& val)
{
	for (int i = 0, n = val.size(); i < n; ++i)
	{
		const Json::Value& src = val[i];
		Slot dst;
		dst.name = src["name"].asString();
		dst.bone = src["bone"].asString();
		dst.attachment = src["attachment"].asString();
		m_slots.push_back(dst);
	}
}

void SpineParser::ParseSkins(const Json::Value& val)
{
	Json::Value::Members key_skins = val.getMemberNames();
	for (int i = 0, n = key_skins.size(); i < n; ++i)
	{
		const std::string& key = key_skins[i];
		const Json::Value& src_skin = val[key];
		Skin skin;
		Json::Value::Members key_items = src_skin.getMemberNames();
		for (int j = 0, m = key_items.size(); j < m; ++j)
		{
			const std::string& key = key_items[j];
			const Json::Value& src_item = src_skin[key];
			SkinItem item;
			item.name = key;
			if (src_item.isMember("type")) {
				item.type = src_item["type"].asString();
			}
			item.pos.x = src_item["x"].asDouble();
			item.pos.y = src_item["y"].asDouble();
			item.angle = src_item["rotation"].asDouble();
			skin.items.push_back(item);
		}
		m_skins.insert(std::make_pair(key, skin));
	}
}

}