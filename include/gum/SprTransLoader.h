#ifndef _GUM_SPR_TRANS_LOADER_H_
#define _GUM_SPR_TRANS_LOADER_H_

#include <sprite2/s2_typedef.h>

namespace simp { class NodeTrans; }

namespace gum
{

class SprTransLoader
{
public:
	static void Load(const s2::SprPtr& spr, const simp::NodeTrans* trans);

private:
	static float ToFloat(int i);

}; // SprTransLoader

}

#endif // _GUM_SPR_TRANS_LOADER_H_