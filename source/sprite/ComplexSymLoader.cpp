#include "ComplexSymLoader.h"
#include "FilepathHelper.h"
#include "SpriteFactory.h"
#include "SprTransLoader.h"

#include <sprite2/S2_Sprite.h>
#include <sprite2/RenderColor.h>
#include <sprite2/ComplexSymbol.h>
#include <simp/NodeComplex.h>

#include <fstream>

namespace gum
{

ComplexSymLoader::ComplexSymLoader(s2::ComplexSymbol* sym, bool flatten)
	: m_sym(sym)
	, m_flatten(flatten)
{
	if (m_sym) {
		m_sym->AddReference();
	}
}

ComplexSymLoader::~ComplexSymLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
}

void ComplexSymLoader::LoadJson(const std::string& filepath)
{
	if (!m_sym) {
		return;
	}

	Json::Value value;
	Json::Reader reader;
	std::locale::global(std::locale(""));
	std::ifstream fin(filepath.c_str());
	std::locale::global(std::locale("C"));
	reader.parse(fin, value);
	fin.close();

	sm::rect scissor;
	scissor.xmin = static_cast<float>(value["xmin"].asInt());
	scissor.xmax = static_cast<float>(value["xmax"].asInt());
	scissor.ymin = static_cast<float>(value["ymin"].asInt());
	scissor.ymax = static_cast<float>(value["ymax"].asInt());
	m_sym->SetScissor(scissor);

	std::string dir = FilepathHelper::Dir(filepath);

	m_sym->Clear();
	for (int i = 0, n = value["sprite"].size(); i < n; ++i) {
		s2::Sprite* spr = SpriteFactory::Instance()->Create(value["sprite"][i], dir);
		m_sym->Add(spr);
		spr->RemoveReference();
	}

//	LoadJsonAction(value, m_sym);
}

void ComplexSymLoader::LoadBin(const simp::NodeComplex* node)
{
	sm::rect scissor;
	scissor.xmin = node->scissor[0];
	scissor.ymin = node->scissor[1];
	scissor.xmax = node->scissor[2];
	scissor.ymax = node->scissor[3];
	m_sym->SetScissor(scissor);

	m_sym->Clear();
	for (int i = 0; i < node->sprs_n; ++i) {
		s2::Sprite* spr = SpriteFactory::Instance()->Create(node->sprs[i], m_flatten);
		SprTransLoader::Load(spr, node->trans[i]);
		m_sym->Add(spr);
		spr->RemoveReference();
	}

	const std::vector<s2::Sprite*>& children = m_sym->GetAllChildren();
	std::vector<s2::ComplexSymbol::Action> dst;
	dst.reserve(node->actions_n);
	for (int i = 0; i < node->actions_n; ++i) {
		const simp::NodeComplex::Action& src_action = node->actions[i];
		s2::ComplexSymbol::Action dst_action;
		dst_action.name = src_action.name;
		dst_action.sprs.reserve(src_action.n);
		for (int j = 0; j < src_action.n; ++j) {
			dst_action.sprs.push_back(children[src_action.idx[j]]);
		}
		dst.push_back(dst_action);
	}
	m_sym->SetActions(dst);
}

//void ComplexSymLoader::LoadJsonAction(const Json::Value& val, s2::ComplexSymbol* sym)
//{
//	const std::vector<s2::Sprite*>& children = sym->GetChildren();
//
//	std::vector<Action> src;
//	LoadJsonAction(val, src);
//	std::vector<s2::ComplexSymbol::Action> dst;
//	dst.reserve(src.size());
//	for (int i = 0, n = src.size(); i < n; ++i) {
//		const Action& src_action = src[i];
//		s2::ComplexSymbol::Action dst_action;
//		dst_action.name = src_action.name;
//		dst_action.sprs.reserve(src_action.idx.size());
//		for (int j = 0, m = src_action.idx.size(); j < m; ++j) {
//			int idx = src_action.idx[j];
//			if (idx >= 0) {
//				dst_action.sprs.push_back(children[idx]);
//			}
//		}
//		dst.push_back(dst_action);
//	}
//	sym->SetActions(dst);
//}
//
//void ComplexSymLoader::LoadJsonAction(const Json::Value& val, std::vector<Action>& actions)
//{
//	if (!val.isMember("action")) {
//		return;
//	}
//
//	const Json::Value& actions_val = val["action"];
//	for (int i = 0, n = actions_val.size(); i < n; ++i) {
//		const Json::Value& src = actions_val[i];
//		Action dst;
//		dst.name = src["name"].asString();
//		dst.idx.reserve(src["sprite"].size());
//		for (int j = 0, m = src["sprite"].size(); j < m; ++j) {
//			dst.idx.push_back(src["sprite"][j].asInt());
//		}
//		actions.push_back(dst);
//	}
//}

}