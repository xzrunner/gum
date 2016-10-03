#include "SpineParser.h"

#include <sm_const.h>

#include <fstream>

#include <assert.h>

namespace gum
{

void SpineParser::Parse(const Json::Value& val)
{
	Clear();

	ParseHeader(val["skeleton"]);
	ParseBones(val["bones"]);
	ParseSlots(val["slots"]);
	ParseSkins(val["skins"]["default"]);
	if (m_parse_anim) {
		ParseAnimations(val["animations"]);
	}
}

const SpineParser::SkinItem* SpineParser::QuerySkin(const Slot& slot) const
{
	std::map<std::string, Skin>::const_iterator itr = skins.find(slot.name);
	assert(itr != skins.end());
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
	bones.clear();
	slots.clear();
	skins.clear();
}

void SpineParser::ParseHeader(const Json::Value& val)
{
	img_dir = "." + val["images"].asString();
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
		dst.angle = src["rotation"].asDouble() * SM_DEG_TO_RAD;
		bones.insert(std::make_pair(dst.name, dst));
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
		slots.push_back(dst);
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
			item.angle = src_item["rotation"].asDouble() * SM_DEG_TO_RAD;
			skin.items.push_back(item);
		}
		skins.insert(std::make_pair(key, skin));
	}
}

void SpineParser::ParseAnimations(const Json::Value& val)
{
	Json::Value::Members key_anis = val.getMemberNames();
	anims.reserve(key_anis.size());
	for (int i = 0, n = key_anis.size(); i < n; ++i)
	{
		const std::string& key = key_anis[i];
		const Json::Value& src = val[key];
		Animation dst;
		dst.name = key;
		Json::Value::Members key_bones = src.getMemberNames();
		dst.bones.reserve(key_bones.size());
		for (int j = 0, m = key_bones.size(); j < m; ++j)
		{
			const std::string& key = key_bones[i];
			AnimBone bone;
			bone.name = key;
			ParseAnimBond(src[key], bone);
			dst.bones.push_back(bone);
		}
		anims.push_back(dst);
	}
}

void SpineParser::ParseAnimBond(const Json::Value& val, AnimBone& bone)
{
	if (val.isMember("rotate"))
	{
		bone.rotates.reserve(val["rotate"].size());
		for (int i = 0, n = val["rotate"].size(); i < n; ++i)
		{
			const Json::Value& src = val["rotate"][i];
			Rotate dst;
			dst.time = src["time"].asDouble();
			dst.rot = src["angle"].asDouble();
			bone.rotates.push_back(dst);
		}
	}
	if (val.isMember("translate"))
	{
		bone.translates.reserve(val["translate"].size());
		for (int i = 0, n = val["translate"].size(); i < n; ++i)
		{
			const Json::Value& src = val["translate"][i];
			Translate dst;
			dst.time = src["time"].asDouble();
			dst.trans.x = src["x"].asDouble();
			dst.trans.y = src["y"].asDouble();
			bone.translates.push_back(dst);
		}
	}
	if (val.isMember("scale"))
	{
		bone.scales.reserve(val["scale"].size());
		for (int i = 0, n = val["scale"].size(); i < n; ++i)
		{
			const Json::Value& src = val["scale"][i];
			Scale dst;
			dst.time = src["time"].asDouble();
			dst.scale.x = src["x"].asDouble();
			dst.scale.y = src["y"].asDouble();
			bone.scales.push_back(dst);
		}
	}
}

}