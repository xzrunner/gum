#ifndef _GUM_SPRITE2_H_
#define _GUM_SPRITE2_H_

#include <CU_Singleton.h>

namespace s2 { class Sprite; }

namespace gum
{

class Sprite2
{
public:
	void DebugDraw() const;

	static void Init();

	static bool IsUseDTex(const s2::Sprite* spr);
	static void SetUseDTex(const s2::Sprite* spr, bool use);

	static bool IsDTexForceCached(const s2::Sprite* spr);
	static void SetDTexForceCached(const s2::Sprite* spr, bool force);
	
	static bool IsDTexForceCachedDirty(const s2::Sprite* spr);
	static void SetDTexForceCachedDirty(const s2::Sprite* spr, bool dirty);

private:
	SINGLETON_DECLARATION(Sprite2)

}; // Sprite2

}

#endif // _GUM_SPRITE2_H_