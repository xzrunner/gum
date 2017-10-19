#ifndef _GUM_BODYMOVIN_ANIM_LOADER_H_
#define _GUM_BODYMOVIN_ANIM_LOADER_H_

#include "BodymovinParser.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>
#include <sprite2/AnimSymbol.h>

namespace s2 { class AnimSymbol; }

namespace gum
{

class SymbolLoader;
class SpriteLoader;

class BodymovinAnimLoader : private cu::Uncopyable
{
public:
	BodymovinAnimLoader(const std::shared_ptr<s2::AnimSymbol>& sym, 
		const std::shared_ptr<const SymbolLoader>& sym_loader = NULL,
		const std::shared_ptr<const SpriteLoader>& spr_loader = NULL);

	void LoadJson(const Json::Value& val, const CU_STR& dir);

	void LoadLayers(const CU_MAP<CU_STR, s2::SprPtr>& map_assets,
		const CU_VEC<BodymovinParser::Layer>& layers, int frame_rate, 
		int width, int height, int start_frame, int end_frame);

private:
	void LoadAssets(CU_MAP<CU_STR, s2::SprPtr>& map_assets,
		const CU_VEC<BodymovinParser::Asset>& assets, int frame_rate, 
		int width, int height, int start_frame, int end_frame);
	void LoadLayers(const CU_MAP<CU_STR, s2::SprPtr>& map_assets,
		const CU_VEC<BodymovinParser::Layer>& layers, int frame_rate, 
		int width, int height, int start_frame, int end_frame, 
		const std::shared_ptr<s2::AnimSymbol>& sym);
	void LoadLayersPrev(const CU_MAP<CU_STR, s2::SprPtr>& map_assets,
		const CU_VEC<BodymovinParser::Layer>& layers, int frame_rate, 
		int width, int height, int start_frame, int end_frame, 
		const std::shared_ptr<s2::AnimSymbol>& sym);
	void LoadLayersPost(const CU_VEC<BodymovinParser::Layer>& layers,
		const std::shared_ptr<s2::AnimSymbol>& sym, int frame_rate, int width, int height,
		int start_frame, int end_frame);
	
	static int Frame2Time(int frame, int frame_rate);

	static void InsertKeyframe(CU_VEC<s2::AnimSymbol::FramePtr>& frames,
		const BodymovinParser::FloatVal& val, int frame_rate, int end_frame);
	static void InsertKeyframe(CU_VEC<s2::AnimSymbol::FramePtr>& frames, int time, int end_time);

	static void LoadAnchor(CU_VEC<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate, int w, int h);	
	static bool LoadOpacity(CU_VEC<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadPosition(CU_VEC<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate, const sm::vec2& left_top);	
	static void LoadRotate(CU_VEC<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadScale(CU_VEC<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	

	static BodymovinParser::FloatVal::Float3 GetLerpVal(
		const CU_VEC<BodymovinParser::FloatVal::KeyFrame>& frames, int frame, int frame_rate);

	s2::SprPtr CreateSolidSpr(const CU_STR& color, int width, int height) const;

	static void LoadBlendMode(CU_VEC<s2::AnimSymbol::FramePtr>& frames, int bm);

	static void LoadIntegrate(CU_VEC<s2::AnimSymbol::FramePtr>& frames);
	
	static void LoadExpression(CU_VEC<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::Transform& trans);

private:
	std::shared_ptr<s2::AnimSymbol> m_sym;

	std::shared_ptr<const SymbolLoader> m_sym_loader;
	std::shared_ptr<const SpriteLoader> m_spr_loader;

}; // BodymovinAnimLoader

}

#endif // _GUM_BODYMOVIN_ANIM_LOADER_H_