#ifndef _GUM_RESOURCE_H_
#define _GUM_RESOURCE_H_

#include <string>

namespace gum
{

class ResPath;

class Resource
{
public:
	virtual ~Resource() {}
	virtual bool LoadFromFile(int pkg_id, const ResPath& res_path, bool async) = 0;

}; // Resource

}

#endif // _GUM_RESOURCE_H_
