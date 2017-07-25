#ifndef _GUM_BODYMOVIN_PARSER_H_
#define _GUM_BODYMOVIN_PARSER_H_

#include <SM_Vector.h>

#include <json/json.h>

#include <string>
#include <map>

#include <string.h>

namespace gum
{

class BodymovinParser
{
public:
	BodymovinParser();

	void Parse(const Json::Value& val, const std::string& dir);

	int GetFrameRate() const { return m_frame_rate; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

public:
	struct Layer;
	struct Asset
	{
		std::string id;

		// single
		std::string filepath;
		int w, h;

		// compose
		std::vector<Layer> layers;
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
		struct Float3
		{
			float data[3];

			Float3();
			Float3(const Json::Value& val);

			bool operator == (const Float3& f) const;
		};

		struct KeyFrame
		{
			int frame;
			Float3 s_val, e_val;
			Float3 ix, iy, ox, oy;
			Float3 ti, to;

			KeyFrame() { memset(this, 0, sizeof(*this)); }
		};

		std::vector<KeyFrame> frames;

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

		// comp
		int comp_width, comp_height;

		// solid type
		int solid_width, solid_height;
		std::string solid_color;

		Transform trans;

		Layer();

		void Load(const Json::Value& val);
	};

public:
	const std::vector<Asset>& GetAssets() const { return m_assets; }
	const std::vector<Layer>& GetLayers() const { return m_layers; }

private:
	void Clear();

	void ParseHeader(const Json::Value& val);
	void ParseAssets(const Json::Value& val, const std::string& dir);
	static void ParseLayers(const Json::Value& val, std::vector<Layer>& layers);

private:
	std::string m_version;
	std::string m_name;

	int m_frame_rate;

	int m_width, m_height;

	int m_start_frame, m_end_frame;

	std::vector<Asset> m_assets;
	std::vector<Layer> m_layers;

}; // BodymovinParser

}

#endif // _GUM_BODYMOVIN_PARSER_H_