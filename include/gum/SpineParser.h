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
	SpineParser(bool parse_anim = true) 
		: m_parse_anim(parse_anim) {}

	void Parse(const Json::Value& val);

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

	struct Rotate
	{
		float time;
		float rot;
	};

	struct Translate
	{
		float time;
		sm::vec2 trans;
	};

	struct Scale
	{
		float time;
		sm::vec2 scale;
	};

	struct AnimBone
	{
		std::string name;
		std::vector<Rotate> rotates;
		std::vector<Translate> translates;
		std::vector<Scale> scales;
	};

	struct Animation
	{
		std::string name;
		std::vector<AnimBone> bones;
	};

	const SkinItem* QuerySkin(const Slot& slot) const;

private:
	void Clear();

	void ParseHeader(const Json::Value& val);
	void ParseBones(const Json::Value& val);
	void ParseSlots(const Json::Value& val);
	void ParseSkins(const Json::Value& val);
	
	void ParseAnimations(const Json::Value& val);
	void ParseAnimBond(const Json::Value& val, AnimBone& bone);

public:
	bool m_parse_anim;

	std::string img_dir;

	std::map<std::string, Bone> bones;

	std::vector<Slot> slots;

	std::map<std::string, Skin> skins;

	std::vector<Animation> anims;

}; // SpineParser

}

#endif // _GUM_SPINE_PARSER_H_