#ifndef _GUM_BODYMOVIN_PARSER_H_
#define _GUM_BODYMOVIN_PARSER_H_

#include <SM_Vector.h>

#include <json/json.h>

#include <string>
#include <map>

namespace gum
{

class BodymovinParser
{
public:
	BodymovinParser();

	void Parse(const Json::Value& val, const std::string& dir);

	int GetFrameRate() const { return m_frame_rate; }

public:
	struct Asset
	{
		std::string filepath;
		int w, h;
	};

	enum LayerType 
	{
		LAYER_PRE_COMP,
		LAYER_SOLID,
		LAYER_IMAGE,
		LAYER_NULL,
		LAYER_SHAPE,
		LAYER_TEXT,
		LAYER_UNKNOWN
	};

	struct FloatVal
	{
		union 
		{
			struct 
			{
				float val[3];
			} RAW;

			struct 
			{
				int start_frame, end_frame;
				float start_val[3], end_val[3];
				float cp1[2], cp2[2];
				float tcp1[2], tcp2[2];
			} KEY;
		} D;

		bool keyframe;

		FloatVal();

		void Load(const Json::Value& val);		
	};

	struct Transform
	{
		FloatVal anchor;
		FloatVal opacity;		

		FloatVal position;
		FloatVal rotate;
		FloatVal scale;

		Transform();

		void Load(const Json::Value& val);
	};

	struct Layer
	{
		std::string name;
		std::string ref_id;

		int layer_id;
		int layer_type;

		std::string cl;	// class

		float in_frame, out_frame;

		int start_frame;

		int auto_ori;	// Auto-Orient along path AE property.
		int blend_mode;

		// solid type
		int solid_width, solid_height;
		std::string solid_color;

		Transform trans;

		Layer();

		void Load(const Json::Value& val);
	};

public:
	const std::vector<Layer>& GetLayers() const { return m_layers; }

	const Asset* QueryAsset(const std::string& id) const;

private:
	void Clear();

	void ParseHeader(const Json::Value& val);
	void ParseAssets(const Json::Value& val, const std::string& dir);
	void ParseLayers(const Json::Value& val);

private:
	std::string m_version;
	std::string m_name;

	int m_frame_rate;

	int m_width, m_height;

	int m_start_frame, m_end_frame;

	std::map<std::string, Asset> m_map2assets;

	std::vector<Layer> m_layers;

}; // BodymovinParser

}

#endif // _GUM_BODYMOVIN_PARSER_H_