#ifndef _GUM_RESOURCE_UID_H_
#define _GUM_RESOURCE_UID_H_

#include <string>

#include <stdint.h>

namespace gum
{

typedef uint64_t UID;

class GlyphStyle;

class ResourceUID
{
public:
	enum Type
	{
		RES_UNKNOWN  = 0,
		RES_BIN_NODE = 1,
		RES_RAW_FILE = 2,
		RES_GLYPH    = 3,
	};
	
public:
	static Type GetType(UID uid);

	static UID RawFile(const std::string& filepath);
	static UID Glyph(int unicode, const GlyphStyle& gs);
	static UID BinNode(uint32_t node_id);

private:
	static const int DATA_SIZE = 62;

	static const uint64_t DATA_MASK = 0x00ffffffffffffff;
	static const uint64_t TYPE_MASK = 0xff00000000000000;

private:
	static uint64_t HashString(const std::string& str);

	static UID Compose(uint64_t data, Type type);

}; // ResourceUID

}

#endif // _GUM_RESOURCE_UID_H_
