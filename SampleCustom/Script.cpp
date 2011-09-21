#include "Script.h"

#include "HookSetup.h"
#include "Scripting.h"
#include "../ScriptHook/Log.h"

#include <windows.h>

#define PrintTxt PrintStringWithLiteralStringNow

// Pull in all our scripting functions/types
using namespace Scripting;
using namespace NativeHooks;


CustomThread::CustomThread()
{
    // Give your own name here!
	SetName("Native Hooks");
}

bool startup = true;

int a2;
float b2, c2, d2;
int* e2;
bool f2;

void PREP_CREATE_CAR(int a, float b, float c, float d, int *e, bool f)
{
	a2 = a;
	b2 = b;
	c2 = c;
	d2 = d;
	e2 = e;
	f2 = f;
}

Scripting::eModel carModel;
bool spawnCar = false;

void CustomThread::RunTick()
{
	// Tick
	if (startup)
	{
		Log::Info(" startup\n");
		startup = false;	
		
		CreateBaseHooks();		
	}
}

int CreateCarCallback(int* carHash, float* X, float* Y, float* Z, int** handle , bool* unk, int previousChanges)
{
	Log::Info("car callback!\n");
	*carHash = MODEL_BANSHEE;
	return 1;
}