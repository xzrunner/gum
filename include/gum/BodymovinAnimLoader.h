#ifndef _GUM_BODYMOVIN_ANIM_LOADER_H_
#define _GUM_BODYMOVIN_ANIM_LOADER_H_

#include "BodymovinParser.h"

#include <CU_Uncopyable.h>
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
	BodymovinAnimLoader(s2::AnimSymbol* sym, const SymbolLoader* sym_loader = NULL,
		const SpriteLoader* spr_loader = NULL);
	~BodymovinAnimLoader();

	void LoadJson(const Json::Value& val, const std::string& dir);

	void LoadLayers(const std::map<std::string, s2::Sprite*>& map_assets,
		const std::vector<BodymovinParser::Layer>& layers, 
		int frame_rate, int width, int height);

private:
	void LoadAssets(std::map<std::string, s2::Sprite*>& map_assets,
		const std::vector<BodymovinParser::Asset>& assets,
		int frame_rate, int width, int height);
	void LoadLayers(const std::map<std::string, s2::Sprite*>& map_assets,
		const std::vector<BodymovinParser::Layer>& layers, 
		int frame_rate, int width, int height, s2::AnimSymbol* sym);

	static int Frame2Time(int frame, int frame_rate);

	static void InsertKeyframe(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);
	static void InsertKeyframe(std::vector<s2::AnimSymbol::Frame*>& frames, int time);

	static void LoadAnchor(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate, int w, int h);	
	static void LoadOpacity(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadPosition(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate, const sm::vec2& left_top);	
	static void LoadRotate(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadScale(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	

	static BodymovinParser::FloatVal::Float3 GetLerpVal(
		const std::vector<BodymovinParser::FloatVal::KeyFrame>& frames, int frame, int frame_rate);

	s2::Sprite* CreateSolidSpr(const std::string& color, int width, int height) const;

	static void LoadBlendMode(std::vector<s2::AnimSymbol::Frame*>& frames, int bm);
	
	static void LoadExpression(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::Transform& trans);

private:
	s2::AnimSymbol* m_sym;

	const SymbolLoader* m_sym_loader;
	const SpriteLoader* m_spr_loader;

}; // BodymovinAnimLoader

}

#endif // _GUM_BODYMOVIN_ANIM_LOADER_H_