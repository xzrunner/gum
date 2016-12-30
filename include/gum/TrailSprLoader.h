#ifndef _GUM_TRAIL_SPR_LOADER_H_
#define _GUM_TRAIL_SPR_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/json.h>

namespace s2 { class TrailSprite; }
namespace simp { class NodeTrailSpr; }

namespace gum
{

class TrailSprLoader : private cu::Uncopyable
{
public:
	TrailSprLoader(s2::TrailSprite* spr);
	~TrailSprLoader();

	void LoadJson(const Json::Value& val, const std::string& dir);
	void LoadBin(const simp::NodeTrailSpr* node);

private:
	s2::TrailSprite* m_spr;

}; // TrailSprLoader

}

#endif // _GUM_TRAIL_SPR_LOADER_H_
