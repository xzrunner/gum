#ifndef _GUM_SHADERLAB_H_
#define _GUM_SHADERLAB_H_

#include <CU_Singleton.h>

namespace gum
{

class ShaderLab
{
public:
	void Init();

	void Update(float dt);

	SINGLETON_DECLARATION(ShaderLab)

}; // ShaderLab

}

#endif // _GUM_SHADERLAB_H_