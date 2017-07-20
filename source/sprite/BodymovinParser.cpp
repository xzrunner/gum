#include "BodymovinParser.h"
#include "FilepathHelper.h"

#include <fstream>

#include <assert.h>
#include <string.h>

namespace gum
{

BodymovinParser::BodymovinParser()
	: m_frame_rate(0)
	, m_width(0)
	, m_height(0)
	, m_start_frame(0)
	, m_end_frame(0)
{
}

void BodymovinParser::Parse(const Json::Value& val, const std::string& dir)
{
	Clear();

	ParseHeader(val);
	ParseAssets(val["assets"], dir);
	ParseLayers(val["layers"]);
}

const BodymovinParser::Asset* BodymovinParser::QueryAsset(const std::string& id) const
{
	std::map<std::string, Asset>::const_iterator itr 
		= m_map2assets.find(id);
	if (itr == m_map2assets.end()) {
		return NULL;
	} else {
		return &itr->second;
	}
}

void BodymovinParser::Clear()
{
	m_map2assets.clear();
	m_layers.clear();
}

void BodymovinParser::ParseHeader(const Json::Value& val)
{
	m_version = val["v"].asString();
	m_name    = val["nm"].asString();

	m_frame_rate = val["fr"].asInt();

	m_width  = val["w"].asInt();
	m_height = val["h"].asInt();

	m_start_frame = val["ip"].asInt();
	m_end_frame   = val["op"].asInt();
}

void BodymovinParser::ParseAssets(const Json::Value& val, const std::string& dir)
{
	for (int i = 0, n = val.size(); i < n; ++i)
	{
		const Json::Value& cval = val[i];

		Asset a;
		a.w = cval["w"].asInt();
		a.h = cval["h"].asInt();
		a.filepath = dir + "\\" + cval["u"].asString() + cval["p"].asString();

		std::string id = cval["id"].asString();

		m_map2assets.insert(std::make_pair(id, a));
	}
}

void BodymovinParser::ParseLayers(const Json::Value& val)
{
	if (val.size() == 0) {
		return;
	}

	m_layers.reserve(val.size());
	for (int i = val.size() - 1; i >= 0; --i) 
	{
		Layer layer;
		layer.Load(val[i]);
		m_layers.push_back(layer);
	}
}

/************************************************************************/
/* class BodymovinParser::FloatVal                                      */
/************************************************************************/

BodymovinParser::FloatVal::FloatVal()
	: keyframe(false)
{
	memset(D.RAW.val, 0, sizeof(D.RAW.val));
}

void BodymovinParser::FloatVal::Load(const Json::Value& val)
{
	keyframe = val["a"].asInt() == 1;
	if (keyframe)
	{
		assert(val["k"].size() == 2);

		int IDX_0 = 0;
		D.KEY.start_frame = val["k"][IDX_0]["t"].asInt();
		for (int i = 0, n = val["k"][IDX_0]["s"].size(); i < n; ++i) {
			D.KEY.start_val[i] = val["k"][IDX_0]["s"][i].asDouble();
			D.KEY.end_val[i] = val["k"][IDX_0]["e"][i].asDouble();
		}

		if (val["k"][IDX_0]["i"]["x"].isArray()) {
			D.KEY.cp1[0] = val["k"][IDX_0]["o"]["x"][IDX_0].asDouble();
			D.KEY.cp1[1] = val["k"][IDX_0]["o"]["y"][IDX_0].asDouble();
			D.KEY.cp2[0] = val["k"][IDX_0]["i"]["x"][IDX_0].asDouble();
			D.KEY.cp2[1] = val["k"][IDX_0]["i"]["y"][IDX_0].asDouble();
			for (int i = 1, n = val["k"][IDX_0]["i"]["x"].size(); i < n; ++i) {
				assert(fabs(val["k"][IDX_0]["o"]["x"][i].asDouble() - D.KEY.cp1[0]) < FLT_EPSILON);
				assert(fabs(val["k"][IDX_0]["o"]["y"][i].asDouble() - D.KEY.cp1[1]) < FLT_EPSILON);
				assert(fabs(val["k"][IDX_0]["i"]["x"][i].asDouble() - D.KEY.cp2[0]) < FLT_EPSILON);
				assert(fabs(val["k"][IDX_0]["i"]["y"][i].asDouble() - D.KEY.cp2[1]) < FLT_EPSILON);
			}
		} else {
 			D.KEY.cp1[0] = val["k"][IDX_0]["o"]["x"].asDouble();
			D.KEY.cp1[1] = val["k"][IDX_0]["o"]["y"].asDouble();
 			D.KEY.cp2[0] = val["k"][IDX_0]["i"]["x"].asDouble();
			D.KEY.cp2[1] = val["k"][IDX_0]["i"]["y"].asDouble();
		}

		for (int i = 0, n = val["k"][IDX_0]["to"].size(); i < n; ++i) {
			D.KEY.tcp1[i] = val["k"][IDX_0]["to"][i].asDouble();
			D.KEY.tcp2[i] = val["k"][IDX_0]["ti"][i].asDouble();
		}

		D.KEY.end_frame = val["k"][1]["t"].asInt();
		assert(!val["k"][1].isMember("e") 
			&& !val["k"][1].isMember("i")
			&& !val["k"][1].isMember("n")
			&& !val["k"][1].isMember("o")
			&& !val["k"][1].isMember("s")
			&& !val["k"][1].isMember("ti")
			&& !val["k"][1].isMember("to"));
	}
	else
	{
		if (val["k"].isArray()) {
			for (int i = 0, n = val["k"].size(); i < n; ++i) {
				D.RAW.val[i] = val["k"][i].asDouble();
			}
		} else {
			D.RAW.val[0] = val["k"].asDouble();
		}
	}
}

/************************************************************************/
/* class BodymovinParser::Transform                                     */
/************************************************************************/

BodymovinParser::Transform::Transform()
{
}

void BodymovinParser::Transform::Load(const Json::Value& val)
{
	if (val.isMember("a")) {
		anchor.Load(val["a"]);
	}
	if (val.isMember("o")) {
		opacity.Load(val["o"]);
	}	

	if (val.isMember("p")) {
		position.Load(val["p"]);
	}
	if (val.isMember("r")) {
		rotate.Load(val["r"]);
	}
	if (val.isMember("s")) {
		scale.Load(val["s"]);
	}
}

/************************************************************************/
/* class BodymovinParser::Layer                                         */
/************************************************************************/

BodymovinParser::Layer::Layer()
{
}

void BodymovinParser::Layer::Load(const Json::Value& val)
{
	name = val["nm"].asString();
	ref_id = val["refId"].asString();

	layer_id = val["ind"].asInt();
	layer_type = val["ty"].asInt();
	if (layer_type == LAYER_SOLID)
	{
		solid_width = val["sw"].asInt();
		solid_height = val["sh"].asInt();
		solid_color = val["sc"].asString();
	}

	cl = val["cl"].asString();

	float time_stretch = 1;
	if (val.isMember("sr")) {
		time_stretch = val["sr"].asDouble();
	}

	in_frame  = val["ip"].asDouble() / time_stretch;
	out_frame = val["op"].asDouble() / time_stretch;

	start_frame = val["st"].asInt();

	auto_ori = val["ao"].asInt();
	blend_mode = val["bm"].asInt();

	trans.Load(val["ks"]);
}

}