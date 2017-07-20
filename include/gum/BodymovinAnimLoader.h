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

private:
	static int Frame2Time(int frame, int frame_rate);

	static void InsertKeyframe(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);
	static void InsertKeyframe(std::vector<s2::AnimSymbol::Frame*>& frames, int time);

	static void LoadAnchor(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadOpacity(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadPosition(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate, const sm::vec2& left_top);	
	static void LoadRotate(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	
	static void LoadScale(std::vector<s2::AnimSymbol::Frame*>& frames, 
		const BodymovinParser::FloatVal& val, int frame_rate);	

private:
	s2::AnimSymbol* m_sym;

	const SymbolLoader* m_sym_loader;
	const SpriteLoader* m_spr_loader;

}; // BodymovinAnimLoader

}

#endif // _GUM_BODYMOVIN_ANIM_LOADER_H_