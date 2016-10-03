#ifndef _GUM_SPINE_PARSER_H_
#define _GUM_SPINE_PARSER_H_

#include <SM_Vector.h>

#include <json/json.h>

#include <string>
#include <map>

namespace gum
{

class SpineParser
{
public:
	SpineParser() {}

	void Parse(const std::string& filepath);

public:
	struct Bone
	{
		std::string name;
		std::string parent;
		sm::vec2 pos;
		float angle;
	};

	struct Slot
	{
		std::string name;
		std::string bone;
		std::string attachment;
	};

	struct SkinItem
	{
		std::string name;
		std::string type;
		sm::vec2 pos;
		float angle;
	};

	struct Skin
	{
		std::string name;
		std::vector<SkinItem> items;
	};

	const SkinItem* QuerySkin(const Slot& slot) const;

private:
	void Clear();

	void ParseHeader(const Json::Value& val);
	void ParseBones(const Json::Value& val);
	void ParseSlots(const Json::Value& val);
	void ParseSkins(const Json::Value& val);

private:
	std::string m_img_dir;

	std::map<std::string, Bone> m_bones;

	std::vector<Slot> m_slots;

	std::map<std::string, Skin> m_skins;

	friend class SpineLoader;

}; // SpineParser

}

#endif // _GUM_SPINE_PARSER_H_