#ifndef _GUM_SHAPE_LOADER_H_
#define _GUM_SHAPE_LOADER_H_

#include <SM_Vector.h>

#include <json/json.h>

#include <memory>

namespace s2 { class Shape; class PolygonShape; class Polygon; }

namespace gum
{

class ShapeLoader
{
public:
	static std::unique_ptr<s2::Shape> LoadShape(const Json::Value& val, const std::string& dir);

private:
	static std::unique_ptr<s2::PolygonShape> LoadPolygon(const Json::Value& val, const std::string& dir);

	static std::unique_ptr<s2::Polygon> LoadPolyMaterial(const Json::Value& val, const std::string& dir,
		const std::vector<sm::vec2>& vertice);

}; // ShapeLoader

}

#endif // _GUM_SHAPE_LOADER_H_