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
	Json::Value::Members key_skins = val["skins"].getMemberNames();
	for (int i = 0, n = key_skins.size(); i < n; ++i) {
		const std::string& key = key_skins[i];
		if (key == "default") {
			continue;
		} else {
			ParseSkins(val["skins"][key]);
			break;
		}
	}
	if (m_parse_anim) {
		ParseAnimations(val["animations"]);
	}
}

const SpineParser::SkinItem* SpineParser::QuerySkin(const Slot& slot) const
{
	const Skin* skin = NULL;
	for (int i = 0, n = skins.size(); i < n; ++i) {
		if (skins[i].name == slot.name) {
			skin = &skins[i];
			break;
		}
	}
	assert(skin);

	for (int i = 0, n = skin->items.size(); i < n; ++i) {
		const SkinItem& item = skin->items[i];
		if (slot.attachment == item.name && item.type != SKIN_BOUNDINGBOX) {
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
	bones.reserve(val.size());
	for (int i = 0, n = val.size(); i < n; ++i)
	{
		const Json::Value& src = val[i];
		Bone dst;
		dst.name = src["name"].asString();
		dst.parent = src["parent"].asString();
		dst.pos.x = src["x"].asDouble();
		dst.pos.y = src["y"].asDouble();
		dst.angle = src["rotation"].asDouble() * SM_DEG_TO_RAD;
		bones.push_back(dst);
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
		skin.name = key;
		Json::Value::Members key_items = src_skin.getMemberNames();
		for (int j = 0, m = key_items.size(); j < m; ++j)
		{
			const std::string& key = key_items[j];
			const Json::Value& src_item = src_skin[key];
			SkinItem item;
			item.name = key;
			if (src_item.isMember("name")) {
				item.path = src_item["name"].asString();
			} else {
				item.path = key;
			}

			item.type = SKIN_IMAGE;
			if (src_item.isMember("type")) {
				std::string stype = src_item["type"].asString();
				if (stype == "boundingbox") {
					item.type = SKIN_BOUNDINGBOX;
				} else if (stype == "mesh") {
					item.type = SKIN_MESH;
				} else {
					item.type = SKIN_UNKNOWN;
				}
			}

			item.width = src_item["width"].asInt();
			item.height = src_item["height"].asInt();

			switch (item.type)
			{
			case SKIN_IMAGE:
				ParseImage(item, src_item);
				break;
			case SKIN_MESH:
				ParseMesh(item, src_item);
				break;
			}

			skin.items.push_back(item);
		}
		skins.push_back(skin);
	}
}

void SpineParser::ParseImage(SkinItem& dst, const Json::Value& src)
{
	dst.pos.x = src["x"].asDouble();
	dst.pos.y = src["y"].asDouble();
	dst.angle = src["rotation"].asDouble() * SM_DEG_TO_RAD;
}

void SpineParser::ParseMesh(SkinItem& dst, const Json::Value& src)
{
	int ptr = 0;
	int tn = src["uvs"].size() / 2;
	dst.texcoords.reserve(tn);
	for (int i = 0; i < tn; ++i) 
	{
		sm::vec2 pos;
		pos.x = src["uvs"][ptr++].asDouble();
		pos.y = src["uvs"][ptr++].asDouble();
		pos.y = 1 - pos.y;
		dst.texcoords.push_back(pos);
	}

	if (src["vertices"].size() == src["uvs"].size())
	{
		int ptr = 0;
		int vn = tn;
		dst.vertices.reserve(vn);
		for (int i = 0; i < vn; ++i) 
		{
			sm::vec2 pos;
			pos.x = src["vertices"][ptr++].asDouble();
			pos.y = src["vertices"][ptr++].asDouble();
			dst.vertices.push_back(pos);
		}
		assert(dst.vertices.size() == dst.texcoords.size());
	}
	else
	{
		for (int i = 0, n = src["vertices"].size(); i < n; )
		{
			SkinItem::SkinnedVertex sv;
			int bone_n = src["vertices"][i++].asInt();
			sv.items.reserve(bone_n);
			for (int j = 0; j < bone_n; ++j) 
			{
				SkinItem::SkinnedVertex::Item item;
				item.bone = src["vertices"][i++].asInt();
				item.vx = src["vertices"][i++].asDouble();
				item.vy = src["vertices"][i++].asDouble();
				item.weight = src["vertices"][i++].asDouble();
				sv.items.push_back(item);
			}
			dst.skinned_vertices.push_back(sv);
		}
		assert(dst.skinned_vertices.size() == dst.texcoords.size());
	}

	ptr = 0;
	dst.triangles.reserve(src["triangles"].size());
	for (int i = 0, n = src["triangles"].size(); i < n; ++i) {
		dst.triangles.push_back(src["triangles"][ptr++].asInt());
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

		Json::Value::Members key_bones = src["bones"].getMemberNames();
		dst.bones.reserve(key_bones.size());
		for (int j = 0, m = key_bones.size(); j < m; ++j)
		{
			const std::string& key = key_bones[j];
			AnimBone bone;
			bone.name = key;
			ParseAnimBond(src["bones"][key], bone);
			dst.bones.push_back(bone);
		}

		Json::Value::Members key_slots = src["slots"].getMemberNames();
		dst.slots.reserve(key_slots.size());
		for (int j = 0, m = key_slots.size(); j < m; ++j)
		{
			const std::string& key = key_slots[j];
			AnimSlot slot;
			slot.name = key;
			ParseAnimSlot(src["slots"][key]["attachment"], slot);
			dst.slots.push_back(slot);
		}

		Json::Value::Members key_deform = src["deform"].getMemberNames();
		if (key_deform.size() > 0)
		{
			ParseAnimDeforms(src["deform"]["default"], dst.deforms);
			if (key_deform.size() > 1) {
				const std::string& key = key_deform[1];
				ParseAnimDeforms(src["deform"][key], dst.deforms);
			}
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
			dst.rot = src["angle"].asDouble() * SM_DEG_TO_RAD;
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

void SpineParser::ParseAnimSlot(const Json::Value& val, AnimSlot& slot)
{
	slot.skins.reserve(val.size());
	for (int i = 0, n = val.size(); i < n; ++i) {
		float time = val[i]["time"].asDouble();
		std::string skin = val[i]["name"].asString();
		slot.skins.push_back(std::make_pair(time, skin));
	}
}

void SpineParser::ParseAnimDeforms(const Json::Value& val, std::vector<AnimDeform>& deforms)
{
	Json::Value::Members key_slots = val.getMemberNames();
	for (int i = 0, n = key_slots.size(); i < n; ++i)
	{
		const std::string& slot = key_slots[i];
		Json::Value::Members key_skins = val[slot].getMemberNames();
		for (int j = 0, m = key_skins.size(); j < m; ++j)
		{
			const std::string& skin = key_skins[j];
			AnimDeform deform;
			deform.slot = slot;
			deform.skin = skin;
			ParseAnimDeform(val[slot][skin], deform);
			deforms.push_back(deform);
		}
	}
}

void SpineParser::ParseAnimDeform(const Json::Value& val, AnimDeform& deform)
{
	deform.samples.reserve(val.size());
	for (int i = 0, n = val.size(); i < n; ++i) 
	{
		const Json::Value& src = val[i];
		Deform dst;

		dst.time = src["time"].asDouble();
		if (src.isMember("offset")) {
			dst.offset = src["offset"].asInt() / 2;
		} else {
			dst.offset = 0;
		}

		int ptr = 0;
		int m = src["vertices"].size() / 2;
		dst.vertices.reserve(m);
		for (int j = 0; j < m; ++j) {
			float x = src["vertices"][ptr++].asDouble(),
				  y = src["vertices"][ptr++].asDouble();
			dst.vertices.push_back(sm::vec2(x, y));
		}

		deform.samples.push_back(dst);
	}
}

}