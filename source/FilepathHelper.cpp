#include "gum/FilepathHelper.h"
#include "gum/StringHelper.h"

#ifndef NO_BOOST
#include <boost/filesystem.hpp>
#endif // NO_BOOST

#include <assert.h>

namespace gum
{

CU_STR FilepathHelper::Relative(const CU_STR& base, const CU_STR& path)
{
#ifndef NO_BOOST
	return boost::filesystem::relative(path.c_str(), base.c_str()).string().c_str();
#else
	assert(0);
	return "";
#endif // NO_BOOST
}

CU_STR FilepathHelper::Absolute(const CU_STR& base, const CU_STR& path)
{
#ifndef NO_BOOST
	return boost::filesystem::absolute(path.c_str(), base.c_str()).string().c_str();
#else
	assert(0);
	return "";
#endif // NO_BOOST
}

CU_STR FilepathHelper::Dir(const CU_STR& path)
{
#ifndef NO_BOOST
	return boost::filesystem::path(path.c_str()).parent_path().string().c_str();
#else
	assert(0);
	return "";
#endif // NO_BOOST
}

CU_STR FilepathHelper::Filename(const CU_STR& path)
{
#ifndef NO_BOOST
	return boost::filesystem::path(path.c_str()).filename().string().c_str();
#else
	assert(0);
	return "";
#endif // NO_BOOST
}

CU_STR FilepathHelper::Format(const CU_STR& path)
{	
#ifndef NO_BOOST
	CU_STR ret = path;

	if (Exists(ret)) {
		ret = boost::filesystem::canonical(ret.c_str()).string().c_str();
	}

//	char from = '\\', to = '/';
	char from = '/', to = '\\';
	std::replace(ret.begin(), ret.end(), from, to);

	StringHelper::ToLower(ret);

	return ret;
#else
	assert(0);
	return "";
#endif // NO_BOOST
}

bool FilepathHelper::Exists(const CU_STR& path)
{
#ifndef NO_BOOST
	return boost::filesystem::exists(path.c_str());
#else
	assert(0);
	return "";
#endif // NO_BOOST
}

}