#include "SymbolPool.h"
#include "gum/StringHelper.h"
#include "SymbolFactory.h"
#include "GUM_ImageSymbol.h"
#include "Image.h"

#include <logger.h>
#include <sprite2/S2_Symbol.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/SymbolVisitor.h>

namespace gum
{

SINGLETON_DEFINITION(SymbolPool);

SymbolPool::SymbolPool()
{
}

void SymbolPool::GC()
{
	while (true)
	{
		bool dirty = false;

		std::map<std::string, s2::Symbol*>::iterator itr0 = m_path_cache.begin();
		while (itr0 != m_path_cache.end())
		{
			if (itr0->second->GetRefCount() == 1) {
				itr0->second->RemoveReference();
				m_path_cache.erase(itr0++);
				dirty = true;
			} else {
				++itr0;
			}
		}

		std::map<uint32_t, s2::Symbol*>::iterator itr1 = m_id_cache.begin();
		while (itr1 != m_id_cache.end())
		{
			if (itr1->second->GetRefCount() == 1) {
				itr1->second->RemoveReference();
				m_id_cache.erase(itr1++);
				dirty = true;
			} else {
				++itr1;
			}
		}

		if (!dirty) {
			 break;
		}
	}
}

s2::Symbol* SymbolPool::Fetch(const std::string& filepath, int type)
{
	std::string fixed_path = filepath;
	StringHelper::ToLower(fixed_path);
	std::map<std::string, s2::Symbol*>::const_iterator itr 
		= m_path_cache.find(fixed_path);
	if (itr != m_path_cache.end()) {
		itr->second->AddReference();
		return itr->second;
	}

	s2::Symbol* ret = SymbolFactory::Instance()->Create(filepath, type);
	if (ret) {
		ret->AddReference();
		m_path_cache.insert(std::make_pair(fixed_path, ret));
	}
	return ret;
}

s2::Symbol* SymbolPool::Fetch(const uint32_t id, bool flatten)
{
	std::map<uint32_t, s2::Symbol*>::const_iterator itr = m_id_cache.find(id);
	if (itr != m_id_cache.end()) {
		itr->second->AddReference();
		return itr->second;
	}

	s2::Symbol* ret = SymbolFactory::Instance()->Create(id, flatten);
	if (ret) {
		ret->AddReference();
		m_id_cache.insert(std::make_pair(id, ret));
	}
	return ret;
}

void SymbolPool::Clear()
{
	std::map<std::string, s2::Symbol*>::iterator itr = m_path_cache.begin();
	for ( ; itr != m_path_cache.end(); ++itr) {
		itr->second->RemoveReference();
	}
	m_path_cache.clear();

	std::map<uint32_t, s2::Symbol*>::iterator itr2 = m_id_cache.begin();
	for ( ; itr2 != m_id_cache.end(); ++itr2) {
		itr2->second->RemoveReference();
	}
	m_id_cache.clear();
}

void SymbolPool::PrintSymRef(uint32_t sym_id) const
{
	class Visitor : public s2::SymbolVisitor
	{
	public:
		Visitor(uint32_t sym_id) 
			: m_sym_id(sym_id), m_finded(false) {}

		virtual void Visit(s2::Sprite* spr) const {
			if (spr->GetSymbol()->GetID() == m_sym_id) {
				m_finded = true;
			}
		}

		bool Finded() const { return m_finded; }

	private:
		uint32_t m_sym_id;

		mutable bool m_finded;

	}; // Visitor

	LOGD("+++ query sym %d ref begin\n", sym_id);
	std::map<uint32_t, s2::Symbol*>::const_iterator itr 
		= m_id_cache.begin();
	for ( ; itr != m_id_cache.end(); ++itr) 
	{
		s2::Symbol* sym = itr->second;
		Visitor visitor(sym_id);
		sym->Traverse(visitor);
		if (visitor.Finded()) {
			LOGD("+++ sym_id %d\n", sym->GetID());
		}
	}
	LOGD("+++ query sym %d ref end\n", sym_id);
}

void SymbolPool::PrintImgRef(const Image* img) const
{
	class Visitor : public s2::SymbolVisitor
	{
	public:
		Visitor(const Image* img) 
			: m_img(img), m_finded(false) {}

		virtual void Visit(s2::Sprite* spr) const 
		{
			if (spr->GetSymbol()->Type() != s2::SYM_IMAGE) {
				return;
			}
			const gum::ImageSymbol* img_sym = VI_DOWNCASTING<const gum::ImageSymbol*>(spr->GetSymbol());
			if (img_sym->GetImage() == m_img) {
				m_finded = true;
			}
		}

		bool Finded() const { return m_finded; }

	private:
		const Image* m_img;

		mutable bool m_finded;

	}; // Visitor

//	LOGD("+++ query img %s ref begin\n", img->GetFilepath().c_str());
	std::map<uint32_t, s2::Symbol*>::const_iterator itr 
		= m_id_cache.begin();
	for ( ; itr != m_id_cache.end(); ++itr) 
	{
		s2::Symbol* sym = itr->second;
		Visitor visitor(img);
		sym->Traverse(visitor);
		if (visitor.Finded()) {
			LOGD("+++ sym_id %d\n", sym->GetID());
		}
	}
//	LOGD("+++ query img %s ref end\n", img->GetFilepath().c_str());
}

}