#include "EasyAnimLoader.h"
#include "SpriteFactory.h"
#include "SprTransLoader.h"
#include "SpriteLoader.h"

#include <sprite2/AnimSymbol.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/LerpCircle.h>
#include <sprite2/LerpSpiral.h>
#include <sprite2/AnimLerp.h>
#include <simp/NodeAnimation.h>
#include <simp/from_int.h>

namespace gum
{

EasyAnimLoader::EasyAnimLoader(s2::AnimSymbol* sym, bool flatten, const SpriteLoader* spr_loader)
	: m_sym(sym)
	, m_spr_loader(spr_loader)
	, m_flatten(flatten)
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
				s2::Sprite* spr = SpriteFactory::Instance()->Create(src_actor->sym_id, m_flatten);
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
			for (int i = 0, n = frame_val["lerp"].size(); i < n; ++i)
			{
				s2::AnimLerp::SprData key = s2::AnimLerp::SprData(frame_val["lerp"][i]["key"].asInt());
				const Json::Value& val = frame_val["lerp"][i]["val"];
				if (val["type"].asString() == "circle")
				{
					float scale = val["scale"].asInt() * 0.01f;
					s2::LerpCircle* circle = new s2::LerpCircle(scale);
					dst_frame->lerps.push_back(std::make_pair(key, circle));
				}
				else if (val["type"].asString() == "spiral") 
				{
					float begin = val["angle_begin"].asInt() * SM_DEG_TO_RAD,
						end   = val["angle_end"].asInt() * SM_DEG_TO_RAD;
					float scale = val["scale"].asInt() * 0.01f;
					s2::LerpSpiral* spiral = new s2::LerpSpiral(begin, end, scale);
					dst_frame->lerps.push_back(std::make_pair(key, spiral));
				}
			}
			int actor_n = frame_val["actor"].size();
			dst_frame->sprs.reserve(actor_n);
			for (int actor = 0; actor < actor_n; ++actor)
			{
				const Json::Value& actor_val = frame_val["actor"][actor];
				s2::Sprite* spr = m_spr_loader->Create(actor_val, dir);
				dst_frame->sprs.push_back(spr);
			}
			dst_layer->frames.push_back(dst_frame);
		}
		m_sym->AddLayer(dst_layer);
	}
}

}