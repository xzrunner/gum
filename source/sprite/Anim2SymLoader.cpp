#include "Anim2SymLoader.h"
#include "FilepathHelper.h"
#include "EasySkeletonLoader.h"
#include "SpineSkeletonLoader.h"

#include <sprite2/SkeletonSymbol.h>

#include <fstream>

namespace gum
{

Anim2SymLoader::Anim2SymLoader(s2::SkeletonSymbol* sym, 
							   const SpriteLoader* spr_loader,
							   const JointLoader* joint_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
	, m_joint_loader(joint_loader)
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
		SpineSkeletonLoader loader(m_sym, m_spr_loader, m_joint_loader);
		loader.LoadJson(val, dir);
	} else {
		EasySkeletonLoader loader(m_sym, m_spr_loader, m_joint_loader);
		loader.LoadJson(val, dir);
	}
}

}