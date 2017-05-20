#include "SymbolFile.h"
#include "gum/StringHelper.h"

#include <sprite2/SymType.h>

#include <assert.h>

namespace gum
{

SINGLETON_DEFINITION(SymbolFile);

const std::string SymbolFile::UNKNOWN_TAG = "unknown";

SymbolFile::SymbolFile()
{
	Regist(s2::SYM_SCALE9,		"scale9");
	Regist(s2::SYM_ICON,		"icon");
	Regist(s2::SYM_TEXTURE,		"texture");
	Regist(s2::SYM_TEXTBOX,		"text");
	Regist(s2::SYM_COMPLEX,		"complex");
	Regist(s2::SYM_ANIMATION,	"anim");
	Regist(s2::SYM_PARTICLE3D,	"particle");
	Regist(s2::SYM_PARTICLE2D,	"particle2d");
	Regist(s2::SYM_SHAPE,		"shape");
	Regist(s2::SYM_MESH,		"mesh");
	Regist(s2::SYM_MASK,		"mask");
	Regist(s2::SYM_TRAIL,		"trail");
	Regist(s2::SYM_SKELETON,	"skeleton");
}

int SymbolFile::Type(const std::string& filepath) const
{
	if (filepath.empty()) {
		return s2::SYM_INVALID;
	}

	int pos = filepath.rfind('.');
	if (pos == -1) {
		return s2::SYM_INVALID;
	}

	std::string ext = filepath.substr(pos + 1);
	StringHelper::ToLower(ext);
	if (ext == "png" || ext == "jpg" || ext == "bmp" || ext == "ppm" || ext == "pvr" || ext == "pkm") 
	{
		return s2::SYM_IMAGE;
	}
	else if (ext == "json") 
	{
		const std::string filename = filepath.substr(0, filepath.find_last_of('.'));
		int pos = filename.find_last_of('_');
		if (pos == -1) {
			return s2::SYM_UNKNOWN;
		}

		std::string tag = filename.substr(pos + 1);
		StringHelper::ToLower(tag);
		std::map<std::string, int>::const_iterator itr = m_tag2type.find(tag);
		if (itr != m_tag2type.end()) {
			return itr->second;
		} else {
			return s2::SYM_UNKNOWN;
		}
	}
	return s2::SYM_UNKNOWN;
}

const std::string& SymbolFile::Tag(int type) const
{
	std::map<int, std::string>::const_iterator itr = m_type2tag.find(type);
	if (itr != m_type2tag.end()) {
		return itr->second;
	} else {
		return UNKNOWN_TAG;
	}
}

void SymbolFile::Regist(int type, const std::string& tag)
{
	m_type2tag.insert(std::make_pair(type, tag));
	m_tag2type.insert(std::make_pair(tag, type));
}

}