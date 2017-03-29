#include "FilepathHelper.h"
#include "StringHelper.h"

#ifndef _LR_CLIENT
	#include <boost/filesystem/operations.hpp>
	#include <boost/algorithm/string.hpp>
#endif

#include <assert.h>

namespace gum
{

std::string FilepathHelper::Relative(const std::string& base, const std::string& path)
{
	#ifndef _LR_CLIENT
		return boost::filesystem::relative(path, base).string();
	#else
		assert(0);
	#endif
}

std::string FilepathHelper::Absolute(const std::string& base, const std::string& path)
{
	#ifndef _LR_CLIENT
		return boost::filesystem::absolute(path, base).string();
	#else
		assert(0);
	#endif;
}

std::string FilepathHelper::Dir(const std::string& path)
{
	#ifndef _LR_CLIENT
		return boost::filesystem::path(path).parent_path().string();
	#else
		assert(0);
	#endif;
}

std::string FilepathHelper::Format(const std::string& path)
{	
	#ifndef _LR_CLIENT
		std::string ret = path;

		if (Exists(ret)) {
			ret = boost::filesystem::canonical(ret).string();
		}

	//	char from = '\\', to = '/';
		char from = '/', to = '\\';
		std::replace(ret.begin(), ret.end(), from, to);

		StringHelper::ToLower(ret);

		return ret;
	#else
		assert(0);
	#endif
}

bool FilepathHelper::Exists(const std::string& path)
{
	#ifndef _LR_CLIENT
		return boost::filesystem::exists(path);
	#else
		assert(0);
	#endif
}

}