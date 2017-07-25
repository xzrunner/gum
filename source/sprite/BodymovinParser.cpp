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
	ParseLayers(val["layers"], m_layers);
}

void BodymovinParser::Clear()
{
	m_assets.clear();
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

		a.id = cval["id"].asString();

		if (cval.isMember("layers"))
		{
			ParseLayers(cval["layers"], a.layers);
		}
		else
		{
			a.w = cval["w"].asInt();
			a.h = cval["h"].asInt();
			a.filepath = dir + "\\" + cval["u"].asString() + cval["p"].asString();
		}

		m_assets.push_back(a);
	}
}

void BodymovinParser::ParseLayers(const Json::Value& val, std::vector<Layer>& layers)
{
	if (val.size() == 0) {
		return;
	}

	layers.reserve(val.size());
	for (int i = val.size() - 1; i >= 0; --i) 
	{
		Layer layer;
		layer.Load(val[i]);
		layers.push_back(layer);
	}
}

/************************************************************************/
/* class BodymovinParser::FloatVal::Float3                              */
/************************************************************************/

BodymovinParser::FloatVal::Float3::Float3()
{
	memset(data, 0, sizeof(data));
}

BodymovinParser::FloatVal::Float3::Float3(const Json::Value& val)
{
	memset(data, 0, sizeof(data));

	if (val.isArray()) {
		assert(val.size() <= 3);
		int n = std::min(3, static_cast<int>(val.size()));
		for (int i = 0; i < n; ++i) {
			data[i] = val[i].asDouble();
		}		
	} else {
		data[0] = val.asDouble();
	}
}

bool BodymovinParser::FloatVal::Float3::operator == (const Float3& f) const
{
	for (int i = 0; i < 3; ++i) {
		if (fabs(data[i] - f.data[i]) > FLT_EPSILON) {
			return false;
		}
	}
	return true;
}

/************************************************************************/
/* class BodymovinParser::FloatVal                                      */
/************************************************************************/

void BodymovinParser::FloatVal::Load(const Json::Value& val)
{
	bool anim = val["a"].asInt() == 1;
	if (!anim) 
	{
		KeyFrame kf;
		kf.s_val = Float3(val["k"]);
		frames.push_back(kf);
		return;
	}

	int n = val["k"].size();
	frames.resize(n);
	for (int i = 0; i < n; ++i)
	{
		const Json::Value& src = val["k"][i];
		KeyFrame& dst = frames[i];
		
		dst.frame = src["t"].asInt();

		if (src.isMember("s")) {
			dst.s_val = Float3(src["s"]);
		}
		if (src.isMember("e")) {
			dst.e_val = Float3(src["e"]);
			if (i != n - 1) {
				frames[i + 1].s_val = dst.e_val;
			}
		}

		if (src.isMember("i")) {
			assert(src["i"].isMember("x") && src["i"].isMember("y"));
			dst.ix = Float3(src["i"]["x"]);
			dst.iy = Float3(src["i"]["y"]);
		}
		if (src.isMember("o")) {
			assert(src["o"].isMember("x") && src["o"].isMember("y"));
			dst.ox = Float3(src["o"]["x"]);
			dst.oy = Float3(src["o"]["y"]);
		}
		if (src.isMember("ti")) {
			dst.ti = Float3(src["ti"]);
		}
		if (src.isMember("to")) {
			dst.to = Float3(src["to"]);
		}
	}

#ifndef NDEBUG
	if (n > 1) {
		for (int i = 0; i < n - 1; ++i) {
			assert(frames[i].e_val == frames[i + 1].s_val);
		}
	}
#endif // NDEBUG
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
	switch (layer_type)
	{
	case LAYER_PRE_COMP:
		comp_width = val["w"].asInt();
		comp_height = val["h"].asInt();
		break;
	case LAYER_SOLID:
		solid_width = val["sw"].asInt();
		solid_height = val["sh"].asInt();
		solid_color = val["sc"].asString();
		break;
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