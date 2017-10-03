#ifndef _GUM_EXTEND_SYM_FILE_H_
#define _GUM_EXTEND_SYM_FILE_H_

#include <cu/cu_macro.h>

#include <json/json.h>

namespace gum
{

enum ExtendSymType
{
	SYM_UNKNOWN,
	SYM_SPINE,
	SYM_BODYMOVIN,
};

class ExtendSymFile
{
public:
	static int GetType(const Json::Value& val);

}; // ExtendSymFile

}

#endif // _GUM_EXTEND_SYM_FILE_H_