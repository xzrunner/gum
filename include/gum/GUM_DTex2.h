#ifndef _GUM_DTEX2_H_
#define _GUM_DTEX2_H_

#include <CU_Singleton.h>

namespace gum
{

class DTex2
{
public:
	void InitHook(void (*draw_begin)(), void (*draw_end)());

	void CreatePkg(int pkg_id);

	void Clear();

	void DebugDraw() const;

private:	
	SINGLETON_DECLARATION(DTex2)

}; // DTex2

}

#endif // _GUM_DTEX2_H_
