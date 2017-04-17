#ifndef _GUM_RESOURCE_MANAGER_H_
#define _GUM_RESOURCE_MANAGER_H_

#include <map>

namespace gum
{

template<class T>
class ResourceManager
{
public:
	T* Create(const std::string& filepath, bool async = false);

	T* Query(const std::string& filepath);
	bool Add(const std::string& filepath, T* res);
	bool Delete(const std::string& filepath);

	void Clear();

	void GC();
	int Count() const { return m_res_map.size(); }

	static ResourceManager* Instance();

private:
	ResourceManager();
	~ResourceManager();

private:
	std::map<std::string, T*> m_res_map;

	static ResourceManager<T>* m_instance;

}; // ResourceManager

}

#include "ResourceManager.inl"

#endif // _GUM_RESOURCE_MANAGER_H_
