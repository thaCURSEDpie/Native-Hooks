////////////////////////////////////////////////////////////////
//		File name:  HookSetup.cpp
//		Part of:    Native-Hooks
//		Author:		thaCURSEDpie
//		Date:		September 2011
//
//		Description:
//		This source file handles the implementation of hooks
//		for native functions. It creates instances of BaseHook
//		which re-route code to functions in this file
//		(these functions should be named accordingly:
//		"NI_<Native name>". Example: "NI_CREATE_CAR".
//
////////////////////////////////////////////////////////////////

#include "HookSetup.h"
#include "HookObjects.h"
#include "NativeContextEditable.h"

// Pull in all our scripting functions/types
using namespace Scripting;

// We store the type of native in this variable upon calling our detour function
//		TODO: check if race conditions are dangerous
u32 nativeParam = 0;

namespace NativeHooks
{
	void EnableHook(Scripting::NativeHashes type);
	void DisableHook(Scripting::NativeHashes type);

	BaseHook **baseHooks;
	int numBaseHooks = 0;
	
	//////////////
	// UNUSED
	//////////////
	u32* getParamAddress()
	{
		return &nativeParam;
	}

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

	//////////////
	// UNUSED
	//////////////
	int PreHook_CreateCar(NCB_CREATE_CAR cbFn)
	{
		
		int j = getBaseHookIndex(NATIVE_CREATE_CAR);
		if (j != -1)
		{
			Log::Debug("Prehook: j == %i\n", j);
			baseHooks[j]->AddCallback(cbFn);
			return 0;
		}
		return 1;
	}

	//////////////
	// UNUSED
	//////////////
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
	
	//////////////
	// UNUSED
	//////////////
	void NI_GENERAL(GameTypes::scrNativeCallContext *cxt)
	{
		u32 param = 0;

		_asm MOV param, EAX;

		Log::Info("param: %u", param);
		// Log::Info("Native hooked!");

		// We get what native called us:
		// NativeHashes type = (NativeHashes)nativeParam;

		// Log::Info("Native type: %u", type);		
		
		// We get a pointer to corresponding base hook:
		// BaseHook* bh = baseHooks[getBaseHookIndex(type)];

		/*
		// TODO: sort out the parameters nicely
		char* params = bh->ParamTypes();

		int ints[10];
		size_t intCount = 0;

		u32 uints[10];
		size_t uintCount = 0;

		float floats[10];
		size_t floatCount = 0;

		b8 bools[10];
		size_t boolCount = 0;

		// iterate through all parameters
		for (u32 i = 0; i < cxt->GetArgCount(); i++)
		{
			switch (bh->ParamTypes()[i])
			{
			case 'i':
				int tempInt;
				cxt->GetArgValue<int>(i, &tempInt);
				ints[intCount++] = tempInt;

				break;

			case 'u':
				u32 tempUint;
				cxt->GetArgValue<u32>(i, &tempUint);
				uints[uintCount++] = tempUint;

				break;

			case 'f':
				float tempFloat;
				cxt->GetArgValue<float>(i, &tempFloat);
				floats[floatCount++] = tempFloat;

				break;

			case 'b':
				bool tempBool;
				cxt->GetArgValue<bool>(i, &tempBool);
				bools[boolCount++] = tempBool;

				break;
			}
		}

		// TODO: call registered funcs
		*/

		BaseHook* bh = baseHooks[getBaseHookIndex(NATIVE_CREATE_CAR)];

		bh->DeActivate();

		NativeCall call = (NativeCall)bh->FunctionAddress();
		call(cxt);

		bh->Activate();
	}

	////////////////////////////////////////////////////
	//
	//		This is a "Native-Intermediate" (NI), the
	//		hooked function is detoured to this function,
	//      which then provides functions registered to
	//		the appropriate BaseHook with the necessary
	//		info.
	////////////////////////////////////////////////////
	void NI_CREATE_CAR(NativeContextEditable *cxt)
	{
		PrintStringWithLiteralStringNow("STRING", "[hook]: hooked!",  1800, true);

		Log::Debug("==Native CREATE_CAR hooked==");

		int j = getBaseHookIndex(NATIVE_CREATE_CAR);
		BaseHook bh = *baseHooks[j];

		Log::Debug("  hook index: %i", j);

		Log::Debug(" de-activating...");
		bh.DeActivate();
		
		Log::Debug(" calling native...");
		NativeCall call = (NativeCall)bh.FunctionAddress();
		call(cxt);

		Log::Debug(" activating...");
		bh.Activate();
		Log::Debug("==END==");
	}
	
	void CreateBaseHooks()
	{
		baseHooks = new BaseHook*[200];

		Log::Debug("Create base hooks :)\n");
		BaseHook *bh = new BaseHook(NATIVE_CREATE_CAR, NI_CREATE_CAR);
		baseHooks[numBaseHooks++] = bh;
		Log::Debug("creating base hook done\n");


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
			Log::Debug("enabling hook...");
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
