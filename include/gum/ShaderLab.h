#ifndef _GUM_SHADERLAB_H_
#define _GUM_SHADERLAB_H_

#include <cu/cu_macro.h>

namespace gum
{

class ShaderLab
{
public:
//	void Init();

	void Update(float dt);

	static void Init();

private:
	CU_SINGLETON_DECLARATION(ShaderLab)

}; // ShaderLab

}

#endif // _GUM_SHADERLAB_H_