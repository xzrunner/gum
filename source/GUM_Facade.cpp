#include "GUM_Facade.h"
#include "DTexC2Strategy.h"
#include "GUM_AsyncTask.h"
#include "SymbolPool.h"
#include "GUM_GTxt.h"
#include "GUM_Model3.h"

namespace gum
{

void Facade::Init()
{
	Model3::Instance();
}

void Facade::Clear()
{
	GTxt::Instance()->Clear();
	DTexC2Strategy::Instance()->Clear();
	AsyncTask::Instance()->Clear();
	SymbolPool::Instance()->Clear();
	// todo: clear sprites
}

}