#ifndef _GUM_RESOURCE_MANAGER_H_
#define _GUM_RESOURCE_MANAGER_H_

#include "ResPath.h"

#include <map>

namespace gum
{

template<class T>
class ResourceManager
{
public:
	T* Create(int pkg_id, const ResPath& res_path, bool async = false);

	T* Query(const ResPath& res_path);
	bool Add(const ResPath& res_path, T* res);
	bool Delete(const ResPath& res_path);

	void Clear();

	void GC();
	int Count() const { return m_res_map.size(); }

	static ResourceManager* Instance();

private:
	ResourceManager();
	~ResourceManager();

private:
	std::map<ResPath, T*> m_res_map;

	static ResourceManager<T>* m_instance;

}; // ResourceManager

}

#include "ResourceManager.inl"

#endif // _GUM_RESOURCE_MANAGER_H_
