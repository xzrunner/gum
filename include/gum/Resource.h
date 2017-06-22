#ifndef _GUM_RESOURCE_H_
#define _GUM_RESOURCE_H_

#include <CU_RefCountObj.h>

#include <string>

namespace gum
{

class ResPath;

class Resource : public cu::RefCountObj
{
public:
	virtual ~Resource() {}
	virtual bool LoadFromFile(const ResPath& res_path, bool async) = 0;

}; // Resource

}

#endif // _GUM_RESOURCE_H_
