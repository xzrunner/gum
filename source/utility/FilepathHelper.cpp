#include "FilepathHelper.h"
#include "StringHelper.h"

#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

namespace gum
{

std::string FilepathHelper::Relative(const std::string& base, const std::string& path)
{
	return boost::filesystem::relative(path, base).string();
}

std::string FilepathHelper::Absolute(const std::string& base, const std::string& path)
{
	return boost::filesystem::absolute(path, base).string();
}

std::string FilepathHelper::Dir(const std::string& path)
{
	return boost::filesystem::path(path).parent_path().string();
}

std::string FilepathHelper::Format(const std::string& path)
{	
	std::string ret = path;

	if (Exists(ret)) {
		ret = boost::filesystem::canonical(ret).string();
	}

//	char from = '\\', to = '/';
	char from = '/', to = '\\';
	std::replace(ret.begin(), ret.end(), from, to);

	StringHelper::ToLower(ret);

	return ret;
}

bool FilepathHelper::Exists(const std::string& path)
{
	return boost::filesystem::exists(path);
}

}