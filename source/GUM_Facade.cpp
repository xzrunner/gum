#include "GUM_Facade.h"
#include "DTexC2Strategy.h"
#include "SymbolPool.h"
#include "GUM_GTxt.h"
#ifndef S2_DISABLE_MODEL
#include "GUM_Model3.h"
#endif // S2_DISABLE_MODEL

namespace gum
{

void Facade::Init()
{
#ifndef S2_DISABLE_MODEL
	Model3::Instance();
#endif // S2_DISABLE_MODEL
}

void Facade::Clear()
{
	GTxt::Instance()->Clear();
	DTexC2Strategy::Instance()->Clear();
	SymbolPool::Instance()->Clear();
	// todo: clear sprites
}

}