#ifndef _GUM_RESOURCE_MANAGER_INL_
#define _GUM_RESOURCE_MANAGER_INL_

namespace gum
{

template<class T>
ResourceManager<T>* ResourceManager<T>::m_instance = NULL;

template<class T>
inline ResourceManager<T>* ResourceManager<T>::Instance()
{
	if (!m_instance)
	{
		m_instance = new ResourceManager<T>;
	}
	return m_instance;
}

template<class T>
inline ResourceManager<T>::ResourceManager()
{
}

template<class T>
inline ResourceManager<T>::~ResourceManager()
{
	Clear();
}

template<class T>
inline T* ResourceManager<T>::Create(const std::string& filepath, bool async)
{
	typename std::map<std::string, T*>::iterator itr = m_res_map.find(filepath);
	if (itr == m_res_map.end())
	{
		T* res = new T;
		bool loaded = res->LoadFromFile(filepath, async);
		if (loaded)
		{
			m_res_map.insert(std::make_pair(filepath, res));
			res->AddReference();
			return res;
		}
		else
		{
			delete res;
			return NULL;
		}
	}
	else
	{
		itr->second->AddReference();
		return itr->second;
	}
}

template<class T>
inline T* ResourceManager<T>::Query(const std::string& filepath)
{
	typename std::map<std::string, T*>::iterator itr = m_res_map.find(filepath);
	if (itr != m_res_map.end()) {
		return itr->second;
	} else {
		return NULL;
	}
}

template<class T>
inline bool ResourceManager<T>::Add(const std::string& filepath, T* res)
{
	std::pair<typename std::map<std::string, T*>::iterator, bool> ret 
		= m_res_map.insert(std::make_pair(filepath, res));
	if (ret.second) {
		res->AddReference();
	}
	return ret.second;
}

template<class T>
inline bool ResourceManager<T>::Delete(const std::string& filepath)
{
	typename std::map<std::string, T*>::iterator itr = m_res_map.find(filepath);
	if (itr == m_res_map.end()) {
		return false;
	}

	itr->second->RemoveReference();
	m_res_map.erase(itr->second);
	return true;
}

template<class T>
inline void ResourceManager<T>::GC()
{
	while (true)
	{
		bool dirty = false;

		typename std::map<std::string, T*>::iterator itr = m_res_map.begin();
		while (itr != m_res_map.end())
		{
			if (itr->second->GetRefCount() == 1) {
				itr->second->RemoveReference();
				m_res_map.erase(itr++);
				dirty = true;
			} else {
				++itr;
			}
		}

		if (!dirty) {
			break;
		}
	}
}

template<class T>
inline void ResourceManager<T>::Clear()
{
	typename std::map<std::string, T*>::iterator itr = m_res_map.begin();
	for ( ; itr != m_res_map.end(); ++itr) {
		itr->second->RemoveReference();
	}
	m_res_map.clear();
}

}

#endif // _GUM_RESOURCE_MANAGER_INL_
