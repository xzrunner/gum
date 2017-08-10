#include "BodymovinAnimLoader.h"
#include "SymbolLoader.h"
#include "SpriteLoader.h"
#include "gum/StringHelper.h"

#include <sm_const.h>
#include <logger.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/ImageSymbol.h>
#include <sprite2/RenderColor.h>
#include <sprite2/ShapeSymbol.h>
#include <sprite2/ColorPolygon.h>
#include <sprite2/PolygonShape.h>
#include <sprite2/BoundingBox.h>
#include <sprite2/BlendMode.h>
#include <sprite2/RenderShader.h>
#include <sprite2/LerpWiggle.h>
#include <sprite2/LerpEase.h>

#include <cmath>

#include <assert.h>

namespace gum
{

static const float FPS = 30.0f;

BodymovinAnimLoader::BodymovinAnimLoader(s2::AnimSymbol* sym, 
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

BodymovinAnimLoader::~BodymovinAnimLoader()
{
	if (m_sym) {
		m_sym->RemoveReference();
	}
	m_sym_loader->RemoveReference();
	m_spr_loader->RemoveReference();
}

void BodymovinAnimLoader::LoadJson(const Json::Value& val, const std::string& dir)
{
	BodymovinParser parser;
	parser.Parse(val, dir);

	std::map<std::string, s2::Sprite*> map_assets;
	LoadAssets(map_assets, parser.GetAssets(), parser.GetFrameRate(), parser.GetWidth(), parser.GetHeight());
	LoadLayers(map_assets, parser.GetLayers(), parser.GetFrameRate(), parser.GetWidth(), parser.GetHeight(), m_sym);
}

void BodymovinAnimLoader::LoadLayers(const std::map<std::string, s2::Sprite*>& map_assets,
									 const std::vector<BodymovinParser::Layer>& layers, 
									 int frame_rate, int width, int height)
{
	LoadLayers(map_assets, layers, frame_rate, width, height, m_sym);
}

void BodymovinAnimLoader::LoadAssets(std::map<std::string, s2::Sprite*>& map_assets,
									 const std::vector<BodymovinParser::Asset>& assets,
									 int frame_rate, int width, int height)
{
	for (int i = 0, n = assets.size(); i < n; ++i)
	{
		const BodymovinParser::Asset& a = assets[i];
		s2::Sprite* spr = NULL;
		if (a.layers.empty()) 
		{
			spr = m_spr_loader->Create(a.filepath);
		} 
		else 
		{
			s2::Symbol* sym = m_sym_loader->Create(s2::SYM_ANIMATION);
			s2::AnimSymbol* anim_sym = VI_DOWNCASTING<s2::AnimSymbol*>(sym);
			LoadLayers(map_assets, a.layers, frame_rate, width, height, anim_sym);
			spr = m_spr_loader->Create(sym);
		}
		map_assets.insert(std::make_pair(a.id, spr));
	}
}

void BodymovinAnimLoader::LoadLayers(const std::map<std::string, s2::Sprite*>& map_assets,
									 const std::vector<BodymovinParser::Layer>& layers, 
									 int frame_rate, int width, int height, s2::AnimSymbol* sym)
{
	LoadLayersPrev(map_assets, layers, frame_rate, width, height, sym);
	LoadLayersPost(layers, sym, frame_rate, width, height);
}

void BodymovinAnimLoader::LoadLayersPrev(const std::map<std::string, s2::Sprite*>& map_assets,
										 const std::vector<BodymovinParser::Layer>& layers, 
										 int frame_rate, int width, int height, s2::AnimSymbol* sym)
{
	sym->SetFPS(static_cast<int>(FPS));

	sm::vec2 left_top;
	left_top.x = - width * 0.5f;
	left_top.y = height * 0.5f;

	for (int i = 0, n = layers.size(); i < n; ++i)
	{
		const BodymovinParser::Layer& src = layers[i];

		int src_w = 0, src_h = 0;
		if (src.layer_type == BodymovinParser::LAYER_PRE_COMP) {
			src_w = src.comp_width;
			src_h = src.comp_height;
		} else if (src.layer_type == BodymovinParser::LAYER_SOLID) {
			src_w = src.solid_width;
			src_h = src.solid_height;
		}

		s2::Sprite *s_spr = NULL, *e_spr = NULL;
		if (src.layer_type == BodymovinParser::LAYER_PRE_COMP ||
			src.layer_type == BodymovinParser::LAYER_IMAGE)
		{
			std::map<std::string, s2::Sprite*>::const_iterator itr 
				= map_assets.find(src.ref_id);
			assert(itr != map_assets.end());
			s_spr = VI_CLONE(s2::Sprite, itr->second);
			e_spr = VI_CLONE(s2::Sprite, itr->second);
		}
		else if (src.layer_type == BodymovinParser::LAYER_SOLID)
		{
			s_spr = CreateSolidSpr(src.solid_color, src.solid_width, src.solid_height);
			e_spr = VI_CLONE(s2::Sprite, s_spr);
		}
		else if (src.layer_type == BodymovinParser::LAYER_NULL)
		{
			s_spr = CreateSolidSpr("#000000", 1, 1);
			e_spr = VI_CLONE(s2::Sprite, s_spr);
		}

		assert(s_spr && e_spr);

		std::string spr_name = "_" + gum::StringHelper::UTF8ToGBK(src.name);
		s_spr->SetName(spr_name);
		e_spr->SetName(spr_name);

		s2::AnimSymbol::Layer* dst = new s2::AnimSymbol::Layer;

		s2::AnimSymbol::Frame* s_frame = new s2::AnimSymbol::Frame;
		s2::AnimSymbol::Frame* e_frame = new s2::AnimSymbol::Frame;
		dst->frames.push_back(s_frame);
		dst->frames.push_back(e_frame);

		s_frame->sprs.push_back(s_spr);
		e_frame->sprs.push_back(e_spr);

		int start_time;
		if (src.start_frame > src.in_frame) {
			start_time = (int)(std::ceil((float)(src.start_frame) / frame_rate * FPS)) + 1;
		} else {
			start_time = (int)(std::ceil((float)(src.in_frame) / frame_rate * FPS)) + 1;
		}
		int end_time = (int)(std::floor((float)(src.out_frame) / frame_rate * FPS)) + 1;

		s_frame->index = start_time;
		e_frame->index = end_time;

		// insert frame
		s2::Sprite *start_null = NULL, *pre_in_null = NULL;
		if (src.start_frame < src.in_frame) 
		{
			// start frame
			s2::AnimSymbol::Frame* s_frame = new s2::AnimSymbol::Frame;
			dst->frames.insert(dst->frames.begin(), s_frame);
			int s_time = (int)(std::ceil((float)(src.start_frame) / frame_rate * FPS)) + 1;
			s_frame->index = s_time;
			s_frame->tween = true;

			start_null = VI_CLONE(s2::Sprite, s_spr);
			s_frame->sprs.push_back(start_null);
			
			// pre in frame
			s2::AnimSymbol::Frame* p_frame = new s2::AnimSymbol::Frame;
			dst->frames.insert(dst->frames.begin() + 1, p_frame);
			int p_time = (int)(std::ceil((float)(src.in_frame) / frame_rate * FPS)) + 1 - 1;
			p_frame->index = p_time;
			p_frame->tween = true;

			pre_in_null = VI_CLONE(s2::Sprite, s_spr);
			p_frame->sprs.push_back(pre_in_null);
		}
		InsertKeyframe(dst->frames, src.trans.anchor, frame_rate);
		InsertKeyframe(dst->frames, src.trans.opacity, frame_rate);
		InsertKeyframe(dst->frames, src.trans.position, frame_rate);
		InsertKeyframe(dst->frames, src.trans.rotate, frame_rate);
		InsertKeyframe(dst->frames, src.trans.scale, frame_rate);

		// filling frames
		LoadAnchor(dst->frames, src.trans.anchor, frame_rate, src_w, src_h);
		bool opacity = LoadOpacity(dst->frames, src.trans.opacity, frame_rate);
		LoadPosition(dst->frames, src.trans.position, frame_rate, left_top);
		LoadRotate(dst->frames, src.trans.rotate, frame_rate);
		LoadScale(dst->frames, src.trans.scale, frame_rate);

		// fix null spr
		s2::RenderColor col;
		col.SetMul(s2::Color(0, 0, 0, 0));
		if (start_null) {
			start_null->SetColor(col);
			pre_in_null->SetColor(col);
		}

		sym->AddLayer(dst);
	}
}

void BodymovinAnimLoader::LoadLayersPost(const std::vector<BodymovinParser::Layer>& layers,
										 s2::AnimSymbol* sym, int frame_rate, int width, int height)
{
	sm::vec2 left_top;
	left_top.x = - width * 0.5f;
	left_top.y = height * 0.5f;

	const std::vector<s2::AnimSymbol::Layer*>& _layers = sym->GetLayers();

	std::map<int, int> map_layerid2idx;
	for (int i = 0, n = layers.size(); i < n; ++i) {
		map_layerid2idx.insert(std::make_pair(layers[i].layer_id, i));
	}

	std::vector<bool> flags(_layers.size(), false);
	while (true)
	{
		bool fail = false;
		for (int i = 0, n = layers.size(); i < n; ++i)
		{
			const BodymovinParser::Layer& src = layers[i];
			s2::AnimSymbol::Layer* dst = _layers[i];

			int src_w = 0, src_h = 0;
			if (src.layer_type == BodymovinParser::LAYER_PRE_COMP) {
				src_w = src.comp_width;
				src_h = src.comp_height;
			} else if (src.layer_type == BodymovinParser::LAYER_SOLID) {
				src_w = src.solid_width;
				src_h = src.solid_height;
			}

			if (src.parent_id == -1)
			{
				flags[i] = true;
			}
			else
			{
				const BodymovinParser::Layer* parent = NULL;
				int p_idx = -1;
				std::map<int, int>::iterator itr = map_layerid2idx.find(src.parent_id);
				if (itr != map_layerid2idx.end()) {
					p_idx = itr->second;
					parent = &layers[p_idx];
				}
				if (parent) 
				{
					assert(p_idx != -1);
					if (flags[p_idx])
					{
						do 
						{
							InsertKeyframe(dst->frames, parent->trans.anchor, frame_rate);
							//InsertKeyframe(dst->frames, parent->trans.opacity, frame_rate);
							InsertKeyframe(dst->frames, parent->trans.position, frame_rate);
							InsertKeyframe(dst->frames, parent->trans.rotate, frame_rate);
							InsertKeyframe(dst->frames, parent->trans.scale, frame_rate);

							LoadAnchor(dst->frames, parent->trans.anchor, frame_rate, src_w, src_h);
							//LoadOpacity(dst->frames, parent->trans.opacity, frame_rate);
							LoadPosition(dst->frames, parent->trans.position, frame_rate, left_top);
							LoadRotate(dst->frames, parent->trans.rotate, frame_rate);
							LoadScale(dst->frames, parent->trans.scale, frame_rate);

							int pid = parent->parent_id;
							parent = NULL;
							if (pid != -1) {
								std::map<int, int>::iterator itr = map_layerid2idx.find(pid);
								if (itr != map_layerid2idx.end()) {
									parent = &layers[itr->second];
								}
							}
						} while (parent);
					}
					else
					{
						fail = true;
						continue;
					}
				}

				flags[i] = true;
			}
		}
		if (!fail) {
			break;
		}
	}

	for (int i = 0, n = layers.size(); i < n; ++i)
	{
		const BodymovinParser::Layer& src = layers[i];
		s2::AnimSymbol::Layer* dst = _layers[i];

		LoadBlendMode(dst->frames, src.blend_mode);

		bool opacity = false;
		if (dst->frames.size() > 1)
		{
			for (int j = 0, m = dst->frames.size(); j < m; ++j) {
				s2::AnimSymbol::Frame* frame = dst->frames[j];
				for (int k = 0, l = frame->sprs.size(); k < l; ++k) {
					s2::Sprite* spr = frame->sprs[k];
					if (spr->GetColor().GetMul().a != 255) {
						opacity = true;
					}
				}
			}
		}
		if (opacity && src.layer_type == BodymovinParser::LAYER_PRE_COMP) {
			LoadIntegrate(dst->frames);
		}

		LoadExpression(dst->frames, src.trans);	
	}
}

int BodymovinAnimLoader::Frame2Time(int frame, int frame_rate)
{
	return (int)(std::ceil((float)frame / frame_rate * FPS)) + 1;
}

void BodymovinAnimLoader::InsertKeyframe(std::vector<s2::AnimSymbol::Frame*>& frames, 
										 const BodymovinParser::FloatVal& val,
										 int frame_rate)
{
	if (frames.size() < 2) {
		return;
	}
	if (val.frames.size() <= 1) {
		return;
	}
	for (int i = 0, n = val.frames.size(); i < n; ++i) {
		InsertKeyframe(frames, Frame2Time(val.frames[i].frame, frame_rate));
	}
}

void BodymovinAnimLoader::InsertKeyframe(std::vector<s2::AnimSymbol::Frame*>& frames, int time)
{
	if (frames.size() < 2) {
		return;
	}

	if (time <= frames[0]->index ||
		time >= frames[frames.size() - 1]->index) {
		return;
	}

	for (int i = 0, n = frames.size() - 1; i < n; ++i) 
	{
		const s2::AnimSymbol::Frame* frame = frames[i];
		assert(time >= frame->index);
		if (time == frame->index) 
		{
			return;
		} 
		else if (time > frame->index && time < frames[i + 1]->index) 
		{
			const s2::AnimSymbol::Frame* clone_frame = frames[i];
			if (clone_frame->sprs.empty()) {
				clone_frame = frames[i + 1];
				assert(!clone_frame->sprs.empty());
			}
			s2::AnimSymbol::Frame* new_frame = new s2::AnimSymbol::Frame;
			new_frame->index = time;
			new_frame->sprs.reserve(clone_frame->sprs.size());
			for (int j = 0, bm = clone_frame->sprs.size(); j < bm; ++j) {
				s2::Sprite* clone = VI_CLONE(s2::Sprite, clone_frame->sprs[j]);
				new_frame->sprs.push_back(clone);
			}
			frames.insert(frames.begin() + i + 1, new_frame);
			return;
		} 
		else 
		{
			continue;
		}
	}
}

void BodymovinAnimLoader::LoadAnchor(std::vector<s2::AnimSymbol::Frame*>& frames, 
									 const BodymovinParser::FloatVal& val, 
									 int frame_rate, int w, int h)
{
	assert(frames.size() >= 2 && !frames[0]->sprs.empty() && !val.frames.empty());

	sm::vec2 ori_sz;
	const s2::Symbol* sym = frames[0]->sprs[0]->GetSymbol();
	switch (sym->Type())
	{
	case s2::SYM_IMAGE:
		ori_sz = VI_DOWNCASTING<const s2::ImageSymbol*>(sym)->GetNoTrimedSize();
		break;
	case s2::SYM_SHAPE:
	case s2::SYM_ANIMATION:
		ori_sz.x = w;
		ori_sz.y = h;
		break;
	default:
		assert(0);
	}

	if (val.frames.size() > 1)
	{
		int s_time = Frame2Time(val.frames.front().frame, frame_rate);
		int e_time = Frame2Time(val.frames.back().frame, frame_rate);
		sm::vec2 s_val(val.frames.front().s_val.data[0], val.frames.front().s_val.data[1]);
		sm::vec2 e_val(val.frames.back().s_val.data[0], val.frames.back().s_val.data[1]);		
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			BodymovinParser::FloatVal::Float3 data = GetLerpVal(val.frames, frame->index, frame_rate);
			sm::vec2 offset(data.data[0], data.data[1]);
			frame->sprs[0]->SetOffset(offset);
		}
	}
	else
	{
		sm::vec2 offset;
		offset.x = - ori_sz.x * 0.5f + val.frames[0].s_val.data[0];
		offset.y =   ori_sz.y * 0.5f - val.frames[0].s_val.data[1];
		for (int i = 0, n = frames.size(); i < n; ++i) 
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			spr->SetOffset(offset);
		}
	}
}

bool BodymovinAnimLoader::LoadOpacity(std::vector<s2::AnimSymbol::Frame*>& frames, 
									  const BodymovinParser::FloatVal& val, 
									  int frame_rate)
{
	bool ret = false;
	assert(frames.size() >= 2 && !frames[0]->sprs.empty() && !val.frames.empty());
	if (val.frames.size() > 1)
	{
		int s_time = Frame2Time(val.frames.front().frame, frame_rate);
		int e_time = Frame2Time(val.frames.back().frame, frame_rate);
		int s_val = (int)(val.frames.front().s_val.data[0]);
		int e_val = (int)(val.frames.back().s_val.data[0]);
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			BodymovinParser::FloatVal::Float3 data = GetLerpVal(val.frames, frame->index, frame_rate);
			int opacity = data.data[0];
			s2::Sprite* spr = frame->sprs[0];
			s2::RenderColor rc = spr->GetColor();
			s2::Color col = spr->GetColor().GetMul();
			col.a = (uint8_t)(255 * opacity / 100.0f);
			rc.SetMul(col);
			spr->SetColor(rc);

			ret = true;
		}
	}
	else
	{
		int opacity = (int)(val.frames[0].s_val.data[0]);
		for (int i = 0, n = frames.size(); i < n; ++i) 
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];

			s2::RenderColor rc = spr->GetColor();
			s2::Color col = spr->GetColor().GetMul();
			col.a = (uint8_t)(255 * opacity / 100.0f);
			rc.SetMul(col);
			spr->SetColor(rc);

			ret = true;
		}
	}
	return ret;
}

void BodymovinAnimLoader::LoadPosition(std::vector<s2::AnimSymbol::Frame*>& frames, 
									   const BodymovinParser::FloatVal& val, 
									   int frame_rate,
									   const sm::vec2& left_top)
{
	assert(frames.size() >= 2 && !frames[0]->sprs.empty() && !val.frames.empty());
	if (val.frames.size() > 1)
	{
		int s_time = Frame2Time(val.frames.front().frame, frame_rate);
		int e_time = Frame2Time(val.frames.back().frame, frame_rate);
		sm::vec2 s_val(val.frames.front().s_val.data[0], val.frames.front().s_val.data[1]);
		sm::vec2 e_val(val.frames.back().s_val.data[0], val.frames.back().s_val.data[1]);		
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			BodymovinParser::FloatVal::Float3 data = GetLerpVal(val.frames, frame->index, frame_rate);
			s2::Sprite* spr = frame->sprs[0];
			sm::vec2 anchor_pos(data.data[0], data.data[1]);
			anchor_pos.x = left_top.x + anchor_pos.x;
			anchor_pos.y = left_top.y - anchor_pos.y;
			spr->Translate(anchor_pos - (spr->GetPosition() + spr->GetOffset()));
		}
	}
	else
	{
		sm::vec2 anchor_pos;
		anchor_pos.x = left_top.x + val.frames[0].s_val.data[0];
		anchor_pos.y = left_top.y - val.frames[0].s_val.data[1];
		for (int i = 0, n = frames.size(); i < n; ++i) 
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			spr->Translate(anchor_pos - (spr->GetPosition() + spr->GetOffset()));
		}
	}
}

void BodymovinAnimLoader::LoadRotate(std::vector<s2::AnimSymbol::Frame*>& frames, 
									 const BodymovinParser::FloatVal& val, 
									 int frame_rate)
{
	assert(frames.size() >= 2 && !frames[0]->sprs.empty() && !val.frames.empty());
	if (val.frames.size() > 1)
	{
		int s_time = Frame2Time(val.frames.front().frame, frame_rate);
		int e_time = Frame2Time(val.frames.back().frame, frame_rate);
		int s_val = (int)(val.frames.front().s_val.data[0]);
		int e_val = (int)(val.frames.back().s_val.data[0]);
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			BodymovinParser::FloatVal::Float3 data = GetLerpVal(val.frames, frame->index, frame_rate);
			int angle = data.data[0];
			frame->sprs[0]->SetAngle(- angle * SM_DEG_TO_RAD);
		}
	}
	else
	{
		int angle = (int)(val.frames[0].s_val.data[0]);
		for (int i = 0, n = frames.size(); i < n; ++i) 
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			spr->SetAngle(- angle * SM_DEG_TO_RAD);
		}
	}
}

void BodymovinAnimLoader::LoadScale(std::vector<s2::AnimSymbol::Frame*>& frames, 
									const BodymovinParser::FloatVal& val, 
									int frame_rate)
{
	assert(frames.size() >= 2 && !frames[0]->sprs.empty() && !val.frames.empty());
	if (val.frames.size() > 1)
	{
		int s_time = Frame2Time(val.frames.front().frame, frame_rate);
		int e_time = Frame2Time(val.frames.back().frame, frame_rate);
		sm::vec2 s_val(val.frames.front().s_val.data[0], val.frames.front().s_val.data[1]);
		sm::vec2 e_val(val.frames.back().s_val.data[0], val.frames.back().s_val.data[1]);		
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			BodymovinParser::FloatVal::Float3 data = GetLerpVal(val.frames, frame->index, frame_rate);
			sm::vec2 scale(data.data[0], data.data[1]);
			frame->sprs[0]->SetScale(scale / 100.0f);
		}
	}
	else
	{
		sm::vec2 scale(val.frames[0].s_val.data[0], val.frames[0].s_val.data[1]);
		scale /= 100.0f;
		for (int i = 0, n = frames.size(); i < n; ++i) 
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			spr->SetScale(scale);
		}
	}
}

BodymovinParser::FloatVal::Float3 
BodymovinAnimLoader::GetLerpVal(const std::vector<BodymovinParser::FloatVal::KeyFrame>& frames, 
								int frame, int frame_rate)
{
	if (frame <= Frame2Time(frames.front().frame, frame_rate)) 
	{
		return frames.front().s_val;
	} 
	else if (frame >= Frame2Time(frames.back().frame, frame_rate))
	{
		return frames.back().s_val;
	}
	else
	{
		for (int i = 1, n = frames.size(); i < n; ++i) 
		{
			const BodymovinParser::FloatVal::KeyFrame& f = frames[i];
			int time = Frame2Time(f.frame, frame_rate);
			if (frame == time) 
			{
				return f.s_val;
			} 
			else if (frame < time) 
			{
				int s_time = Frame2Time(frames[i - 1].frame, frame_rate);
				int e_time = time;
				BodymovinParser::FloatVal::Float3 ret;
				const BodymovinParser::FloatVal::KeyFrame& pf = frames[i - 1];
				for (int i = 0; i < 3; ++i) {
					ret.data[i] = (f.s_val.data[i] - pf.s_val.data[i]) * (float)(frame - s_time) / (e_time - s_time) + pf.s_val.data[i];
				}
				return ret;
			}
		}
	}

	assert(0);
	return frames.front().s_val;
}

s2::Sprite* BodymovinAnimLoader::CreateSolidSpr(const std::string& color, int width, int height) const
{
	s2::Symbol* sym = m_sym_loader->Create(s2::SYM_SHAPE);
	assert(sym);
	s2::ShapeSymbol* shape_sym = VI_DOWNCASTING<s2::ShapeSymbol*>(sym);

	// set color
	int r, g, b;
	StringHelper::FromString(color.substr(1, 2), r);
	StringHelper::FromString(color.substr(3, 2), g);
	StringHelper::FromString(color.substr(5, 2), b);
	s2::ColorPolygon* poly = new s2::ColorPolygon(s2::Color(r, g, b));

	// region
	float hw = width * 0.5f,
		  hh = height * 0.5f;
	std::vector<sm::vec2> outline(4);
	outline[0].Set(-hw, -hh);
	outline[1].Set( hw, -hh);
	outline[2].Set( hw,  hh);
	outline[3].Set(-hw,  hh);
	poly->SetOutline(outline);
	poly->Build();

	s2::PolygonShape* poly_shape = new s2::PolygonShape();
	poly_shape->SetVertices(outline);
	poly_shape->SetPolygon(poly);
	poly->RemoveReference();
	shape_sym->SetShape(poly_shape);
	poly_shape->RemoveReference();

	s2::Sprite* spr = m_spr_loader->Create(sym);
	spr->UpdateBounding();
	return spr;
}

void BodymovinAnimLoader::LoadBlendMode(std::vector<s2::AnimSymbol::Frame*>& frames, int body_bm)
{
	if (body_bm == 0) {
		return;
	}

	s2::BlendMode bm = s2::BM_NULL;
	switch (body_bm)
	{
	case 0:
		bm = s2::BM_NULL;
		break;
	case 1:
		bm = s2::BM_MULTIPLY;
		break;
	case 2:
		bm = s2::BM_SCREEN;
		break;
	case 3:
		bm = s2::BM_OVERLAY;
		break;
	case 4:
		bm = s2::BM_DARKEN;
		break;
	case 5:
		bm = s2::BM_LIGHTEN;
		break;
	case 6:
		bm = s2::BM_COLOR_DODGE;
		break;
	case 7:
		bm = s2::BM_COLOR_BURN;
		break;
	case 8:
		bm = s2::BM_HARD_LIGHT;
		break;
	case 9:
		bm = s2::BM_SOFT_LIGHT;
		break;
	case 10:
		bm = s2::BM_DIFFERENCE;
		break;
	case 11:
		bm = s2::BM_EXCLUSION;
		break;
	case 12:
		bm = s2::BM_HUE;
		break;
	case 13:
		bm = s2::BM_SATURATION;
		break;
	case 14:
		bm = s2::BM_COLOR;
		break;
	case 15:
		bm = s2::BM_LUMINOSITY;
		break;
	default:
		LOGW("Unknown blend type %d", bm);
		return;
	}

	for (int i = 0, n = frames.size(); i < n; ++i)
	{
		s2::AnimSymbol::Frame* f = frames[i];
		for (int j = 0, m = f->sprs.size(); j < m; ++j) 
		{
			s2::Sprite* spr = f->sprs[j];
			s2::RenderShader rs = spr->GetShader();
			rs.SetBlend(bm);
			spr->SetShader(rs);
		}
	}
}

void BodymovinAnimLoader::LoadIntegrate(std::vector<s2::AnimSymbol::Frame*>& frames)
{
	for (int i = 0, n = frames.size(); i < n; ++i) {
		s2::AnimSymbol::Frame* f = frames[i];
		for (int j = 0, m = f->sprs.size(); j < m; ++j) {
			s2::Sprite* spr = f->sprs[j];
			spr->SetIntegrate(true);
		}
	}
}

void BodymovinAnimLoader::LoadExpression(std::vector<s2::AnimSymbol::Frame*>& frames, 
										 const BodymovinParser::Transform& trans)
{
	if (frames.empty()) {
		return;
	}

	// wiggle
	if (trans.position.expression.find("wiggle") != std::string::npos)
	{
		const std::string& exp = trans.position.expression;
		std::string params = exp.substr(exp.find("wiggle(") + 7);

		int freq, amp;
		sscanf(params.c_str(), "%d, %d", &freq, &amp);
		for (int i = 0, n = frames.size(); i < n; ++i) {
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			for (int j = 0, m = frame->sprs.size(); j < m; ++j) {
				s2::LerpWiggle* lerp = new s2::LerpWiggle(freq, amp);
				frame->lerps.push_back(std::make_pair(s2::AnimLerp::SPR_POS, lerp));
			}
		}
	}

	// in out back
	if (trans.scale.expression.find("easeandwizz_outBack") != std::string::npos)
	{
		for (int i = 0, n = frames.size(); i < n; ++i) {
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			for (int j = 0, m = frame->sprs.size(); j < m; ++j) {
				s2::LerpEase* lerp = new s2::LerpEase(s2::LerpEase::EASE_IN_OUT_BACK);
				frame->lerps.push_back(std::make_pair(s2::AnimLerp::SPR_SCALE, lerp));
			}
		}
	}
}

}