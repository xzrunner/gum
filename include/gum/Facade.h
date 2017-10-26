#ifndef _GUM_FACADE_H_
#define _GUM_FACADE_H_

namespace gum
{

class Facade
{
public:
	static void Initialize();
	static void Terminate();

	static void Clear();

}; // Facade

}

#endif // _GUM_FACADE_H_