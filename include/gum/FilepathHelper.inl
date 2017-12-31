#pragma once

#include <cu/cu_stl.h>

#include <string>
#ifndef NO_BOOST
#include <boost/filesystem.hpp>
#endif // NO_BOOST

namespace gum
{

template<>
inline std::string FilepathHelper::Extension(const std::string& path)
{
	std::string ext;

#ifndef NO_BOOST
	ext = boost::filesystem::extension(boost::filesystem::path(path));
#else
	ext = path.substr(path.find_last_of('.'));
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
#endif // NO_BOOST

	// skip invalid postfix
	if (ext.size() > 16) {
		ext = Extension(path.substr(0, path.find_last_of('.')));
	}

	return ext;
}

template<>
inline CU_STR FilepathHelper::Extension(const CU_STR& path)
{
	CU_STR ext;

#ifndef NO_BOOST
	ext = boost::filesystem::extension(boost::filesystem::path(path.c_str())).c_str();
#else
	ext = path.substr(path.find_last_of('.'));
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
#endif // NO_BOOST

	// skip invalid postfix
	if (ext.size() > 16) {
		ext = Extension(path.substr(0, path.find_last_of('.')));
	}

	return ext;
}

}