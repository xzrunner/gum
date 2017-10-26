#include "gum/Facade.h"
#include "gum/DTexC2Strategy.h"
#include "gum/SymbolPool.h"
#include "gum/GTxt.h"
#ifndef S2_DISABLE_MODEL
// todo
//#include "gum/Model3.h"
#endif // S2_DISABLE_MODEL
#include "gum/ThreadPool.h"
#include "gum/Audio.h"

namespace gum
{

void Facade::Initialize()
{
#ifndef S2_DISABLE_MODEL
	// todo
//	Model3::Instance();
#endif // S2_DISABLE_MODEL

	Audio::Instance()->Initialize(NULL, NULL);
}

void Facade::Terminate()
{
	Audio::Instance()->Terminate();	
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