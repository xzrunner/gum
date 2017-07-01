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
inline T* ResourceManager<T>::Create(const ResPath& res_path, bool async)
{
	typename std::map<ResPath, T*>::iterator itr = m_res_map.find(res_path);
	if (itr == m_res_map.end())
	{
		T* res = new T;
		bool loaded = res->LoadFromFile(res_path, async);
		if (loaded)
		{
			m_res_map.insert(std::make_pair(res_path, res));
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
inline T* ResourceManager<T>::Query(const ResPath& res_path)
{
	typename std::map<ResPath, T*>::iterator itr = m_res_map.find(res_path);
	if (itr != m_res_map.end()) {
		return itr->second;
	} else {
		return NULL;
	}
}

template<class T>
inline bool ResourceManager<T>::Add(const ResPath& res_path, T* res)
{
	std::pair<typename std::map<ResPath, T*>::iterator, bool> ret 
		= m_res_map.insert(std::make_pair(res_path, res));
	if (ret.second) {
		res->AddReference();
	}
	return ret.second;
}

template<class T>
inline bool ResourceManager<T>::Delete(const ResPath& res_path)
{
	typename std::map<ResPath, T*>::iterator itr = m_res_map.find(res_path);
	if (itr == m_res_map.end()) {
		return false;
	}

	itr->second->RemoveReference();
	m_res_map.erase(itr);
	return true;
}

template<class T>
inline void ResourceManager<T>::Clear()
{
	typename std::map<ResPath, T*>::iterator itr = m_res_map.begin();
	for ( ; itr != m_res_map.end(); ++itr) {
		itr->second->RemoveReference();
	}
	m_res_map.clear();
}

template<class T>
inline void ResourceManager<T>::GC()
{
	while (true)
	{
		bool dirty = false;

		typename std::map<ResPath, T*>::iterator itr = m_res_map.begin();
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

}

#endif // _GUM_RESOURCE_MANAGER_INL_
