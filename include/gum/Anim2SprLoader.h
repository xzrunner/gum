#ifndef _GUM_ANIM2_SPR_LOADER_H_
#define _GUM_ANIM2_SPR_LOADER_H_

#include <CU_Uncopyable.h>

#include <json/json.h>

namespace s2 { class Anim2Sprite; }

namespace gum
{

class Anim2SprLoader : private cu::Uncopyable
{
public:
	Anim2SprLoader(s2::Anim2Sprite* spr);
	~Anim2SprLoader();

	void LoadJson(const Json::Value& val, const std::string& dir);

private:
	s2::Anim2Sprite* m_spr;

}; // Anim2SprLoader

}

#endif // _GUM_ANIM2_SPR_LOADER_H_
