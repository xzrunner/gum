#include "SpineAnimLoader.h"
#include "SpineParser.h"
#include "SymbolLoader.h"
#include "SpriteLoader.h"
#include "FilepathHelper.h"

#include <sprite2/S2_Sprite.h>
#include <sprite2/AnimSymbol.h>

namespace gum
{

SpineAnimLoader::SpineAnimLoader(s2::AnimSymbol* sym, 
								 const SymbolLoader* sym_loader,
								 const SpriteLoader* spr_loader)
	: m_sym(sym)
	, m_sym_loader(sym_loader)
	, m_spr_loader(spr_loader)
{
	if (m_sym) {
		m_sym->AddReference();
	}
	if (m_sym_loader) {
		m_sym_loader->AddReference();
	} else {
		m_sym_loader = new SymbolLoader;
	}
	if (m_spr_loader) {
		m_spr_loader->AddReference();
	} else {
		m_spr_loader = new SpriteLoader;
	}
}

SpineAnimLoader::~SpineAnimLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_sym_loader->RemoveReference();
	m_spr_loader->RemoveReference();
}

void SpineAnimLoader::LoadJson(const Json::Value& val, const std::string& dir,
							   const std::string& filepath)
{
//	SpineParser parser;
//	parser.Parse(val);
//
//	std::string img_dir = FilepathHelper::Absolute(dir, parser.img_dir);
//	
//	m_sym->SetFPS(30);
//
//	
////	SpineSkeletonLoader loader();
//
//	s2::AnimSymbol::Layer* layer = new s2::AnimSymbol::Layer;
//	s2::AnimSymbol::Frame* frame = new s2::AnimSymbol::Frame;
//	frame->index = 1;
//	frame->sprs.push_back(m_sym_loader->Load(val, dir));
//
//// 	for (int i = 0, n = parser.slots.size(); i < n; ++i)
//// 	{
//// 		const SpineParser::Slot& slot = parser.slots[i];
//// 		const SpineParser::SkinItem* item = parser.QuerySkin(slot);
//// 		if (!item) {
//// 			continue;
//// 		}
//// 		std::string filepath = FilepathHelper::Absolute(img_dir, item->name + ".png");
//// 		s2::Sprite* spr = m_sym_loader->Load(filepath);
//// 		spr->SetAngle(item->angle);
//// 		spr->SetPosition(item->pos);
//// 		frame->sprs.push_back(spr);
//// 	}
// 	layer->frames.push_back(frame);
//	m_sym->AddLayer(layer);

	//////////////////////////////////////////////////////////////////////////

	m_sym->SetFPS(30);

	s2::Symbol* sym = m_sym_loader->Create(filepath, s2::SYM_SKELETON);
	
	s2::AnimSymbol::Layer* layer = new s2::AnimSymbol::Layer;
	s2::AnimSymbol::Frame* frame = new s2::AnimSymbol::Frame;
	frame->index = 1;
	s2::Sprite* spr = m_spr_loader->Create(sym);
	frame->sprs.push_back(spr);
	layer->frames.push_back(frame);
	m_sym->AddLayer(layer);

	sym->RemoveReference();
}

}