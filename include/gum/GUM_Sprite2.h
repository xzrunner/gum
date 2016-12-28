#ifndef _GUM_SPRITE2_H_
#define _GUM_SPRITE2_H_

#include <CU_Singleton.h>

namespace gum
{

class Sprite2
{
public:
	void DebugDraw() const;

private:
	SINGLETON_DECLARATION(Sprite2)

}; // Sprite2

}

#endif // _GUM_SPRITE2_H_