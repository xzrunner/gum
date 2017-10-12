#ifndef _GUM_BODYMOVIN_ANIM_LOADER_H_
#define _GUM_BODYMOVIN_ANIM_LOADER_H_

#include "BodymovinParser.h"

#include <cu/uncopyable.h>
#include <sprite2/AnimSymbol.h>

#include <string>
#include <vector>

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

	void LoadJson(const Json::Value& val, const std::string& dir);

	void LoadLayers(const std::map<std::string, s2::SprPtr>& map_assets,
		const std::vector<BodymovinParser::Layer>& layers, int frame_rate, 
		int width, int height, int start_frame, int end_frame);

private:
	void LoadAssets(std::map<std::string, s2::SprPtr>& map_assets,
		const std::vector<BodymovinParser::Asset>& assets, int frame_rate, 
		int width, int height, int start_frame, int end_frame);
	void LoadLayers(const std::map<std::string, s2::SprPtr>& map_assets,
		const std::vector<BodymovinParser::Layer>& layers, int frame_rate, 
		int width, int height, int start_frame, int end_frame, 
		const std::shared_ptr<s2::AnimSymbol>& sym);
	void LoadLayersPrev(const std::map<std::string, s2::SprPtr>& map_assets,
		const std::vector<BodymovinParser::Layer>& layers, int frame_rate, 
		int width, int height, int start_frame, int end_frame, 
		const std::shared_ptr<s2::AnimSymbol>& sym);
	void LoadLayersPost(const std::vector<BodymovinParser::Layer>& layers,
		const std::shared_ptr<s2::AnimSymbol>& sym, int frame_rate, int width, int height,
		int start_frame, int end_frame);
	
	static int Frame2Time(int frame, int frame_rate);

	static void InsertKeyframe(std::vector<s2::AnimSymbol::FramePtr>& frames,
		const BodymovinParser::FloatVal& val, int frame_rate, int end_frame);
	static void InsertKeyframe(std::vector<s2::AnimSymbol::FramePtr>& frames, int time, int end_time);

	static void LoadAnchor(std::vector<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate, int w, int h);	
	static bool LoadOpacity(std::vector<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadPosition(std::vector<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate, const sm::vec2& left_top);	
	static void LoadRotate(std::vector<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadScale(std::vector<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	

	static BodymovinParser::FloatVal::Float3 GetLerpVal(
		const std::vector<BodymovinParser::FloatVal::KeyFrame>& frames, int frame, int frame_rate);

	s2::SprPtr CreateSolidSpr(const std::string& color, int width, int height) const;

	static void LoadBlendMode(std::vector<s2::AnimSymbol::FramePtr>& frames, int bm);

	static void LoadIntegrate(std::vector<s2::AnimSymbol::FramePtr>& frames);
	
	static void LoadExpression(std::vector<s2::AnimSymbol::FramePtr>& frames, 
		const BodymovinParser::Transform& trans);

private:
	std::shared_ptr<s2::AnimSymbol> m_sym;

	std::shared_ptr<const SymbolLoader> m_sym_loader;
	std::shared_ptr<const SpriteLoader> m_spr_loader;

}; // BodymovinAnimLoader

}

#endif // _GUM_BODYMOVIN_ANIM_LOADER_H_