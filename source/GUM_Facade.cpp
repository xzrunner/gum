#include "GUM_Facade.h"
#include "DTexC2Strategy.h"
#include "GUM_AsyncTask.h"
#include "SymbolPool.h"

namespace gum
{

void Facade::Clear()
{
	DTexC2Strategy::Instance()->Clear();
	AsyncTask::Instance()->Clear();
	SymbolPool::Instance()->Clear();
	// todo: clear sprites
}

}