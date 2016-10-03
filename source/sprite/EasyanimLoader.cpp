#include "EasyAnimLoader.h"
#include "SpriteFactory.h"
#include "SprTransLoader.h"
#include "SpriteLoader.h"

#include <sprite2/AnimSymbol.h>
#include <sprite2/S2_Sprite.h>
#include <simp/NodeAnimation.h>
#include <simp/from_int.h>

namespace gum
{

EasyAnimLoader::EasyAnimLoader(s2::AnimSymbol* sym, const SpriteLoader* spr_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
{
	if (m_sym) {
		m_sym->AddReference();
	}
	if (m_spr_loader) {
		m_spr_loader->AddReference();
	} else {
		m_spr_loader = new SpriteLoader;
	}
}

EasyAnimLoader::~EasyAnimLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_spr_loader->RemoveReference();
}

void EasyAnimLoader::LoadJson(const Json::Value& val, const std::string& dir)
{
	if (!m_sym) {
		return;
	}

	int fps = val["fps"].asInt();
	m_sym->SetFPS(fps);

	LoadLayers(val["layer"], dir);
}

void EasyAnimLoader::LoadBin(const simp::NodeAnimation* node)
{
	if (!m_sym) {
		return;
	}

	for (int layer = 0; layer < node->n; ++layer)
	{
		const simp::NodeAnimation::Layer* src_layer = &node->layers[layer];
		s2::AnimSymbol::Layer* dst_layer = new s2::AnimSymbol::Layer;
		int frame_n = src_layer->n;
		dst_layer->frames.reserve(frame_n);
		for (int frame = 0; frame < frame_n; ++frame)
		{
			const simp::NodeAnimation::Frame* src_frame = src_layer->frames[frame];
			s2::AnimSymbol::Frame* dst_frame = new s2::AnimSymbol::Frame;
			dst_frame->index = src_frame->index;
			dst_frame->tween = simp::int2bool(src_frame->tween);
			int actor_n = src_frame->n;
			dst_frame->sprs.reserve(actor_n);
			for (int actor = 0; actor < actor_n; ++actor)
			{
				const simp::NodeAnimation::Actor* src_actor = src_frame->actors[actor];
				s2::Sprite* spr = SpriteFactory::Instance()->Create(src_actor->sym_id);
				SprTransLoader::Load(spr, src_actor->trans);
				dst_frame->sprs.push_back(spr);
			}
			dst_layer->frames.push_back(dst_frame);
		}
		m_sym->AddLayer(dst_layer);
	}
}

void EasyAnimLoader::LoadLayers(const Json::Value& val, const std::string& dir)
{
	int layer_n = val.size();
	for (int layer = 0; layer < layer_n; ++layer)
	{
		const Json::Value& layer_val = val[layer];
		s2::AnimSymbol::Layer* dst_layer = new s2::AnimSymbol::Layer;
		dst_layer->name = layer_val["name"].asString();
		int frame_n = layer_val["frame"].size();
		dst_layer->frames.reserve(frame_n);
		for (int frame = 0; frame < frame_n; ++frame)
		{
			const Json::Value& frame_val = layer_val["frame"][frame];
			s2::AnimSymbol::Frame* dst_frame = new s2::AnimSymbol::Frame;
			dst_frame->index = frame_val["time"].asInt();
			dst_frame->tween = frame_val["tween"].asBool();
			int actor_n = frame_val["actor"].size();
			dst_frame->sprs.reserve(actor_n);
			for (int actor = 0; actor < actor_n; ++actor)
			{
				const Json::Value& actor_val = frame_val["actor"][actor];
				s2::Sprite* spr = m_spr_loader->Load(actor_val, dir);
				dst_frame->sprs.push_back(spr);
			}
			dst_layer->frames.push_back(dst_frame);
		}
		m_sym->AddLayer(dst_layer);
	}
}

}