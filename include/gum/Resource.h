#ifndef _GUM_RESOURCE_H_
#define _GUM_RESOURCE_H_

#include <string>

namespace bimp { class FilePath; }

namespace gum
{

class Resource
{
public:
	virtual ~Resource() {}
	virtual bool LoadFromFile(int pkg_id, const bimp::FilePath& res_path, bool async) = 0;

}; // Resource

}

#endif // _GUM_RESOURCE_H_
