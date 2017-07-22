#include "ShapeSaver.h"
#include "JsonSerializer.h"

#include <sprite2/Shape.h>
#include <sprite2/ShapeType.h>
#include <sprite2/PolygonShape.h>
#include <sprite2/Polygon.h>
#include <sprite2/PolyType.h>
#include <sprite2/ColorPolygon.h>

namespace gum
{

void ShapeSaver::Store(const s2::Shape* shape, Json::Value& val)
{
	switch (shape->Type())
	{
	case s2::SHAPE_POLYGON:
		StorePolygon(VI_DOWNCASTING<const s2::PolygonShape*>(shape), val);
		break;
	}
}

void ShapeSaver::StorePolygon(const s2::PolygonShape* poly_shape, Json::Value& val)
{
	val["type"] = "polygon";
		
	const std::vector<sm::vec2>& vertices = poly_shape->GetVertices();
	JsonSerializer::Store(vertices, val["vertices"]);

	Json::Value& mt_val = val["material"];

	const s2::Polygon* poly = poly_shape->GetPolygon();
	switch (poly->Type())
	{
	case s2::POLY_COLOR:
		{
			mt_val["type"] = "color";
			const s2::ColorPolygon* color_poly = VI_DOWNCASTING<const s2::ColorPolygon*>(poly);
			mt_val["color"] = color_poly->GetColor().ToRGBA();
		}
		break;
	case s2::POLY_TEXTURE:
		mt_val["type"] = "texture";
		break;
	}
}

}