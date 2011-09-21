#include "HookSetup.h"
#include "NativeContextEditable.h"

// Pull in all our scripting functions/types
using namespace Scripting;

namespace NativeHooks
{
	void EnableHook(Scripting::NativeHashes type);
	void DisableHook(Scripting::NativeHashes type);

	BaseHook **baseHooks;
	int numBaseHooks = 0;
	
	int getBaseHookIndex(NativeHashes type)
	{
		for (int i = 0; i < numBaseHooks; i++)
		{
			if (baseHooks[i]->Type() == type)
			{
				return i;
			}
		}
		
		return -1;
	}	

	int PreHook_CreateCar(NCB_CREATE_CAR cbFn)
	{
		int j = getBaseHookIndex(NATIVE_CREATE_CAR);
		if (j != -1)
		{
			Log::Info("Prehook: j == %i\n", j);
			baseHooks[j]->AddCallback(cbFn);
			return 0;
		}
		return 1;
	}

	int PostHook_CreateCar(NCB_CREATE_CAR cbFn)
	{
		int j = getBaseHookIndex(NATIVE_CREATE_CAR);
		if (j != -1)
		{
			baseHooks[j]->AddPostHook(cbFn);
			return 0;
		}
		return 1;
	}

	typedef void (_cdecl *NativeCall)(GameTypes::scrNativeCallContext *cxt);

	// void NI_CREATE_CAR(GameTypes::scrNativeCallContext *cxt)
	void NI_CREATE_CAR(NativeContextEditable *cxt)
	{
		int j = getBaseHookIndex(NATIVE_CREATE_CAR);		
		BaseHook bh = *baseHooks[j];

		
		Log::Info("==Native CREATE_CAR hooked! Listing info:==");
		Log::Info(" # args:     %i", cxt->GetArgCount());
		Log::Info(" data count: %i", cxt->GetDataCount());
		Log::Info("u32 representation of all arguments:");

		for (u32 i = 0; i < cxt->GetArgCount(); i++)
		{
			u32 ret;
			cxt->GetArgValue<u32>(i, &ret);

			Log::Info(" #%i    %u", i, ret);
			Log::Info("      0x%x", ret);
		}

		Log::Info("MODEL_BANSHEE in u32 representation:");
		Log::Info("%u", MODEL_BANSHEE);
		Log::Info("==Native CREATE_CAR hook: logging done==");

		cxt->SetArgValue<u32>(0, MODEL_BANSHEE);

		bh.DeActivate();

		NativeCall call = (NativeCall)bh.FunctionAddress();
		call(cxt);

		bh.Activate();

		return;
	}

	/*
	void NI_CREATE_CAR(int a, float b, float c, float d, int *e, bool f)
	{
		Log::Info("NI_CREATE_CAR");

		int g = 0;
		int retVal = 0;

		int j = getBaseHookIndex(NATIVE_CREATE_CAR);
		
		BaseHook bh = *baseHooks[j];

		int a2 = a;
		float b2 = b, c2 = c, d2 = d;
		int* e2 = e;
		bool f2 = f;

		Log::Info(" iterating through children");
		for (int i = 0; i < bh.GetNumCallbacks(); i++)
		{			
			retVal = ((NCB_CREATE_CAR)bh.callbacks[i])(&a, &b, &c, &d, &e, &f, g);
			
			if (retVal == 1 && g != 2)
			{
				g = 1;
			}
			if (retVal == 2)
			{
				g = 2;
			}
		}
		
		bh.DeActivate();
		
		bh.Activate();

		return;
	}
	*/

	void CreateBaseHooks()
	{
		baseHooks = new BaseHook*[200];

		Log::Info("Create base hooks :)\n");
		BaseHook *bh = new BaseHook(NATIVE_CREATE_CAR, NI_CREATE_CAR);
		baseHooks[numBaseHooks++] = bh;
		Log::Info("creating base hook done\n");


		for (int i = 0; i < numBaseHooks; i++)
		{
			baseHooks[i]->Activate();
		}
	}

	bool IsHookEnabled(NativeHashes type)
	{
		int i = getBaseHookIndex(type);

		if (i != -1)
		{
			return baseHooks[i]->IsActivated();
		}

		return true;
	}

	void EnableHook(NativeHashes type)
	{
		int i = getBaseHookIndex(type);

		if (i != -1)
		{
			Log::Info("enabling hook...");
			baseHooks[i]->Activate();
		}
	}

	void DisableHook(NativeHashes type)
	{
		int i = getBaseHookIndex(type);

		if (i != -1)
		{
			baseHooks[i]->DeActivate();
		}
	}


}
