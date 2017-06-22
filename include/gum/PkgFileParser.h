#ifndef _GUM_PKG_FILE_PARSER_H_
#define _GUM_PKG_FILE_PARSER_H_

#include <string>

#include <stdint.h>

struct fs_file;

namespace gum
{

class PkgFileParser
{
public:
	static uint32_t GetEpeIdxOffset(fs_file* file);
	static uint32_t GetEptIdxOffset(fs_file* file);

	static void SetEPPath(fs_file* file, const std::string& filepath, int pkg_id);

}; // PkgFileParser

}

#endif // _GUM_PKG_FILE_PARSER_H_