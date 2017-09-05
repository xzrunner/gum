#include "GUM_Facade.h"
#include "DTexC2Strategy.h"
#include "SymbolPool.h"
#include "GUM_GTxt.h"
#ifndef S2_DISABLE_MODEL
// todo
//#include "GUM_Model3.h"
#endif // S2_DISABLE_MODEL
#include "gum/ThreadPool.h"
#include "gum/GUM_Audio.h"

namespace gum
{

void Facade::Init()
{
#ifndef S2_DISABLE_MODEL
	// todo
//	Model3::Instance();
#endif // S2_DISABLE_MODEL

	Audio::Instance()->InitContext(NULL, NULL);
}

void Facade::Close()
{
	ThreadPool::Instance()->Close();
}

void Facade::Clear()
{
	GTxt::Instance()->Clear();
	DTexC2Strategy::Instance()->Clear();
	SymbolPool::Instance()->Clear();
	// todo: clear sprites
}

}