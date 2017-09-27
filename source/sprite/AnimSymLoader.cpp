#include "AnimSymLoader.h"
#include "FilepathHelper.h"
#include "EasyAnimLoader.h"
#include "SpineAnimLoader.h"
#include "ExtendSymFile.h"
#include "BodymovinAnimLoader.h"

#include <sprite2/AnimSymbol.h>

#include <json/json.h>

#include <fstream>

#include <string.h>

namespace gum
{

AnimSymLoader::AnimSymLoader(s2::AnimSymbol* sym, 
							 const SymbolLoader* sym_loader,
							 const SpriteLoader* spr_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
	, m_sym_loader(sym_loader)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

AnimSymLoader::~AnimSymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

void AnimSymLoader::LoadJson(const std::string& filepath)
{
	if (!m_sym) {
		return;
	}

	std::string dir = FilepathHelper::Dir(filepath);

	Json::Value val;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, val);
	fin.close();

	int type = ExtendSymFile::GetType(val);
	switch (type)
	{
	case SYM_SPINE:
		{
			SpineAnimLoader loader(m_sym, m_sym_loader, m_spr_loader);
			loader.LoadJson(val, dir, filepath);
		}
		break;
	case SYM_BODYMOVIN:
		{
			BodymovinAnimLoader loader(m_sym, m_sym_loader, m_spr_loader);
			loader.LoadJson(val, dir);
		}
		break;
	case SYM_UNKNOWN:
		{
			EasyAnimLoader loader(m_sym, m_spr_loader);
			loader.LoadJson(val, dir);
		}
		break;
	}

	m_sym->LoadCopy();
}

void AnimSymLoader::LoadBin(const simp::NodeAnimation* node)
{
	EasyAnimLoader loader(m_sym, m_spr_loader);
	loader.LoadBin(node);

	m_sym->LoadCopy();
}

}