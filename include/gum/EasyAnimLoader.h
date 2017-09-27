#ifndef _GUM_EASYANIM_LOADER_H_
#define _GUM_EASYANIM_LOADER_H_

#include <CU_Uncopyable.h>
#include <simp/NodeAnimation.h>
#include <sprite2/AnimSymbol.h>

#include <json/json.h>

#include <string>

namespace s2 { class AnimSymbol; class Sprite; }
namespace simp { class NodeAnimation; }

namespace gum
{

class SpriteLoader;

class EasyAnimLoader : private cu::Uncopyable
{
public:
	EasyAnimLoader(s2::AnimSymbol* sym, const SpriteLoader* spr_loader = NULL);
	~EasyAnimLoader();

	void LoadJson(const Json::Value& val, const std::string& dir);
	void LoadBin(const simp::NodeAnimation* node);

private:
	void LoadLayers(const Json::Value& val, const std::string& dir);
 	void LoadActors(const Json::Value& src, s2::AnimSymbol::Frame* dst, const std::string& dir);
 	void LoadLerps(const Json::Value& src, s2::AnimSymbol::Frame* dst);

	void LoadActors(const simp::NodeAnimation::Frame* src, s2::AnimSymbol::Frame* dst);
	void LoadLerps(const simp::NodeAnimation::Frame* src, s2::AnimSymbol::Frame* dst);

private:
	s2::AnimSymbol* m_sym;

	const SpriteLoader* m_spr_loader;

}; // EasyAnimLoader

}

#endif // _GUM_EASYANIM_LOADER_H_