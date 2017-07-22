#ifndef _GUM_SHAPE_SAVER_H_
#define _GUM_SHAPE_SAVER_H_

#include <json/json.h>

namespace s2 { class Shape; class PolygonShape; }

namespace gum
{

class ShapeSaver
{
public:
	static void Store(const s2::Shape* shape, Json::Value& val);

private:
	static void StorePolygon(const s2::PolygonShape* polygon, Json::Value& val);

}; // ShapeSaver

}

#endif // _GUM_SHAPE_SAVER_H_