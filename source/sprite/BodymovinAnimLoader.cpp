#include "BodymovinAnimLoader.h"
#include "SymbolLoader.h"
#include "SpriteLoader.h"

#include <sm_const.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/ImageSymbol.h>
#include <sprite2/RenderColor.h>

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
	m_sym->SetFPS(static_cast<int>(FPS));

	BodymovinParser parser;
	parser.Parse(val, dir);

	int frame_rate = parser.GetFrameRate();

	sm::vec2 left_top;
	left_top.x = - parser.GetWidth() * 0.5f;
	left_top.y = parser.GetHeight() * 0.5f;

	const std::vector<BodymovinParser::Layer>& layers = parser.GetLayers();
	for (int i = 0, n = layers.size(); i < n; ++i)
	{
		const BodymovinParser::Layer& src = layers[i];

		s2::Sprite *s_spr = NULL, *e_spr = NULL;
		if (src.layer_type == BodymovinParser::LAYER_IMAGE)
		{
			const BodymovinParser::Asset* asset = parser.QueryAsset(src.ref_id);
			assert(asset);
			s_spr = m_spr_loader->Create(asset->filepath);
			e_spr = VI_CLONE(s2::Sprite, s_spr);
		}
		else if (src.layer_type == BodymovinParser::LAYER_SOLID)
		{
			// todo
			continue;
		}
		assert(s_spr && e_spr);

		s2::AnimSymbol::Layer* dst = new s2::AnimSymbol::Layer;

		s2::AnimSymbol::Frame* s_frame = new s2::AnimSymbol::Frame;
		s2::AnimSymbol::Frame* e_frame = new s2::AnimSymbol::Frame;
		dst->frames.push_back(s_frame);
		dst->frames.push_back(e_frame);
		
		s_frame->sprs.push_back(s_spr);
		e_frame->sprs.push_back(e_spr);

		int start_time = (int)(std::ceil((float)(src.in_frame) / frame_rate * FPS)) + 1;
		int end_time = (int)(std::floor((float)(src.out_frame) / frame_rate * FPS)) + 1;

		s_frame->index = start_time;
		e_frame->index = end_time;

		// insert frame
		InsertKeyframe(dst->frames, src.trans.anchor, frame_rate);
		InsertKeyframe(dst->frames, src.trans.opacity, frame_rate);
		InsertKeyframe(dst->frames, src.trans.position, frame_rate);
		InsertKeyframe(dst->frames, src.trans.rotate, frame_rate);
		InsertKeyframe(dst->frames, src.trans.scale, frame_rate);

		// filling frames
		LoadAnchor(dst->frames, src.trans.anchor, frame_rate);
		LoadOpacity(dst->frames, src.trans.opacity, frame_rate);
		LoadPosition(dst->frames, src.trans.position, frame_rate, left_top);
		LoadRotate(dst->frames, src.trans.rotate, frame_rate);
		LoadScale(dst->frames, src.trans.scale, frame_rate);

		m_sym->AddLayer(dst);
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

	if (!val.keyframe) {
		return;
	}

	InsertKeyframe(frames, Frame2Time(val.D.KEY.start_frame, frame_rate));
	InsertKeyframe(frames, Frame2Time(val.D.KEY.end_frame, frame_rate));
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
		if (time == frame->index) {
			return;
		} else if (time > frame->index && time < frames[i + 1]->index) {
			s2::AnimSymbol::Frame* new_frame = new s2::AnimSymbol::Frame;
			new_frame->index = time;
			new_frame->sprs.reserve(frame->sprs.size());
			for (int j = 0, m = frame->sprs.size(); j < m; ++j) {
				s2::Sprite* clone = VI_CLONE(s2::Sprite, frame->sprs[j]);
				new_frame->sprs.push_back(clone);
			}
			frames.insert(frames.begin() + i + 1, new_frame);
			return;
		} else {
			continue;
		}
	}
}

void BodymovinAnimLoader::LoadAnchor(std::vector<s2::AnimSymbol::Frame*>& frames, 
									 const BodymovinParser::FloatVal& val, 
									 int frame_rate)
{
	assert(frames.size() >= 2 && !frames[0]->sprs.empty());

	const s2::ImageSymbol* img_sym = VI_DOWNCASTING<const s2::ImageSymbol*>(frames[0]->sprs[0]->GetSymbol());
	sm::vec2 ori_sz = img_sym->GetNoTrimedSize();

	if (val.keyframe)
	{
		int s_time = Frame2Time(val.D.KEY.start_frame, frame_rate);
		int e_time = Frame2Time(val.D.KEY.end_frame, frame_rate);
		sm::vec2 s_val(val.D.KEY.start_val[0], val.D.KEY.start_val[1]);
		sm::vec2 e_val(val.D.KEY.end_val[0], val.D.KEY.end_val[1]);
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];			
			sm::vec2 offset;
			if (frame->index <= s_time) {
				offset = s_val;
			} else if (frame->index >= e_time) {
				offset = e_val;
			} else {
				offset = (e_val - s_val) * (float)(frame->index - s_time) / (e_time - s_time) + s_val;
			}
			spr->SetOffset(offset);
		}
	}
	else
	{
		sm::vec2 offset;
		offset.x = - ori_sz.x * 0.5f + val.D.RAW.val[0];
		offset.y =   ori_sz.y * 0.5f - val.D.RAW.val[1];
		for (int i = 0, n = frames.size(); i < n; ++i) 
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			spr->SetOffset(offset);
		}
	}
}

void BodymovinAnimLoader::LoadOpacity(std::vector<s2::AnimSymbol::Frame*>& frames, 
									  const BodymovinParser::FloatVal& val, 
									  int frame_rate)
{
	assert(frames.size() >= 2 && !frames[0]->sprs.empty());

	const s2::ImageSymbol* img_sym = VI_DOWNCASTING<const s2::ImageSymbol*>(frames[0]->sprs[0]->GetSymbol());
	sm::vec2 ori_sz = img_sym->GetNoTrimedSize();

	if (val.keyframe)
	{
		int s_time = Frame2Time(val.D.KEY.start_frame, frame_rate);
		int e_time = Frame2Time(val.D.KEY.end_frame, frame_rate);
		int s_val = (int)(val.D.KEY.start_val[0]);
		int e_val = (int)(val.D.KEY.end_val[0]);
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			int opacity;
			if (frame->index <= s_time) {
				opacity = s_val;
			} else if (frame->index >= e_time) {
				opacity = e_val;
			} else {
				opacity = (int)((e_val - s_val) * (float)(frame->index - s_time) / (e_time - s_time) + s_val);
			}

			s2::RenderColor rc = spr->GetColor();
			s2::Color col = spr->GetColor().GetMul();
			col.a = (uint8_t)(255 * opacity / 100.0f);
			rc.SetMul(col);
			spr->SetColor(rc);
		}
	}
	else
	{
		int opacity = (int)(val.D.RAW.val[0]);
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
		}
	}
}

void BodymovinAnimLoader::LoadPosition(std::vector<s2::AnimSymbol::Frame*>& frames, 
									   const BodymovinParser::FloatVal& val, 
									   int frame_rate,
									   const sm::vec2& left_top)
{
	assert(frames.size() >= 2 && !frames[0]->sprs.empty());

	const s2::ImageSymbol* img_sym = VI_DOWNCASTING<const s2::ImageSymbol*>(frames[0]->sprs[0]->GetSymbol());
	sm::vec2 ori_sz = img_sym->GetNoTrimedSize();

	if (val.keyframe)
	{
		int s_time = Frame2Time(val.D.KEY.start_frame, frame_rate);
		int e_time = Frame2Time(val.D.KEY.end_frame, frame_rate);
		sm::vec2 s_val(val.D.KEY.start_val[0], val.D.KEY.start_val[1]);
		sm::vec2 e_val(val.D.KEY.end_val[0], val.D.KEY.end_val[1]);
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			sm::vec2 anchor_pos;
			if (frame->index <= s_time) {
				anchor_pos = s_val;
			} else if (frame->index >= e_time) {
				anchor_pos = e_val;
			} else {
				anchor_pos = (e_val - s_val) * (float)(frame->index - s_time) / (e_time - s_time) + s_val;
			}
			anchor_pos.x = left_top.x + anchor_pos.x;
			anchor_pos.y = left_top.y - anchor_pos.y;
			spr->Translate(anchor_pos - (spr->GetPosition() + spr->GetOffset()));
		}
	}
	else
	{
		sm::vec2 anchor_pos;
		anchor_pos.x = left_top.x + val.D.RAW.val[0];
		anchor_pos.y = left_top.y - val.D.RAW.val[1];
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
	assert(frames.size() >= 2 && !frames[0]->sprs.empty());

	const s2::ImageSymbol* img_sym = VI_DOWNCASTING<const s2::ImageSymbol*>(frames[0]->sprs[0]->GetSymbol());
	sm::vec2 ori_sz = img_sym->GetNoTrimedSize();

	if (val.keyframe)
	{
		int s_time = Frame2Time(val.D.KEY.start_frame, frame_rate);
		int e_time = Frame2Time(val.D.KEY.end_frame, frame_rate);
		int s_val = (int)(val.D.KEY.start_val[0]);
		int e_val = (int)(val.D.KEY.end_val[0]);
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			int angle;
			if (frame->index <= s_time) {
				angle = s_val;
			} else if (frame->index >= e_time) {
				angle = e_val;
			} else {
				angle = (int)((e_val - s_val) * (float)(frame->index - s_time) / (e_time - s_time) + s_val);
			}
			spr->SetAngle(- angle * SM_DEG_TO_RAD);
		}
	}
	else
	{
		int angle = (int)(val.D.RAW.val[0]);
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
	assert(frames.size() >= 2 && !frames[0]->sprs.empty());

	const s2::ImageSymbol* img_sym = VI_DOWNCASTING<const s2::ImageSymbol*>(frames[0]->sprs[0]->GetSymbol());
	sm::vec2 ori_sz = img_sym->GetNoTrimedSize();

	if (val.keyframe)
	{
		int s_time = Frame2Time(val.D.KEY.start_frame, frame_rate);
		int e_time = Frame2Time(val.D.KEY.end_frame, frame_rate);
		sm::vec2 s_val(val.D.KEY.start_val[0], val.D.KEY.start_val[1]);
		sm::vec2 e_val(val.D.KEY.end_val[0], val.D.KEY.end_val[1]);
		for (int i = 0, n = frames.size(); i < n; ++i)
		{
			s2::AnimSymbol::Frame* frame = frames[i];
			frame->tween = true;
			assert(frame->sprs.size() == 1);
			s2::Sprite* spr = frame->sprs[0];
			sm::vec2 scale;
			if (frame->index <= s_time) {
				scale = s_val;
			} else if (frame->index >= e_time) {
				scale = e_val;
			} else {
				scale = (e_val - s_val) * (float)(frame->index - s_time) / (e_time - s_time) + s_val;
			}
			spr->SetScale(scale / 100.0f);
		}
	}
	else
	{
		sm::vec2 scale(val.D.RAW.val[0], val.D.RAW.val[1]);
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

}