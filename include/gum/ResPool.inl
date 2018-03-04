#pragma once

namespace gum
{

template <typename T, typename... Arguments>
inline std::shared_ptr<T> ResPool::Fetch(const std::string& filepath, Arguments... parameters)
{
	auto itr = m_path2res.find(filepath);
	if (itr != m_path2res.end()) 
	{
		auto ptr = itr->second.lock();
		if (ptr) {
			return std::static_pointer_cast<T>(ptr);
		} else {
			m_path2res.erase(itr);
		}
	}

	auto ptr = std::make_shared<T>(parameters...);
	ptr->LoadFromFile(filepath);
	m_path2res.insert(std::make_pair(filepath, ptr));
	return ptr;
}

template <typename T, typename... Arguments>
inline std::pair<std::shared_ptr<T>, bool> ResPool::FetchNoLoad(const std::string& filepath, Arguments... parameters)
{
	auto itr = m_path2res.find(filepath);
	if (itr != m_path2res.end()) 
	{
		auto ptr = itr->second.lock();
		if (ptr) {
			return std::make_pair(std::static_pointer_cast<T>(ptr), true);
		} else {
			m_path2res.erase(itr);
		}
	}

	auto ptr = std::make_shared<T>(parameters...);
	m_path2res.insert(std::make_pair(filepath, ptr));
	return std::make_pair(ptr, false);
}

}