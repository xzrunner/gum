#include "Anim2SymLoader.h"
#include "FilepathHelper.h"
#include "EasyAnim2Loader.h"
#include "SpineAnim2Loader.h"

#include <sprite2/Anim2Symbol.h>

#include <fstream>

namespace gum
{

Anim2SymLoader::Anim2SymLoader(s2::Anim2Symbol* sym, 
							   const SymbolLoader* sym_loader)
	: m_sym(sym)
	, m_sym_loader(sym_loader)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

Anim2SymLoader::~Anim2SymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

void Anim2SymLoader::LoadJson(const std::string& filepath)
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

	if (val.isMember("skeleton") && !val["skeleton"].isArray() && val["skeleton"].isMember("spine")) {
		SpineAnim2Loader loader(m_sym, m_sym_loader);
		loader.LoadJson(val, dir);
	} else {
		EasyAnim2Loader loader(m_sym, m_sym_loader);
		loader.LoadJson(val, dir);
	}
}

}