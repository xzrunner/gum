#ifndef _GUM_SPRITE2_H_
#define _GUM_SPRITE2_H_

#include <cu/cu_macro.h>

namespace s2 { class Sprite; }

namespace gum
{

class Sprite2
{
public:
	void DebugDraw() const;

	void Init();

	void Flush();

private:
	static const int MAX_DC_COUNT = 30;
	int m_curr_dc, m_dc_count;

	CU_SINGLETON_DECLARATION(Sprite2)

}; // Sprite2

}

#endif // _GUM_SPRITE2_H_