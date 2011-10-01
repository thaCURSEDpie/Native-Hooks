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
			
	void CreateBaseHooks()
	{
		baseHooks = new BaseHook*[200];

		Log::Debug("Create base hooks :)\n");		
		baseHooks[numBaseHooks++] = new BaseHook(NATIVE_CREATE_CAR);
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

	// Ret vals:
	// 0: success
	// 1: failed to create hook
	int RegisterHook(NativeHashes type, REGISTERED_HOOK* callback)
	{
		int i = getBaseHookIndex(type);

		if (i != -1)
		{
			baseHooks[i]->RegisterHook(callback);
		}
		else
		{
			CreateHook(type);

			i = getBaseHookIndex(type);
			
			if (i != -1)
			{
				baseHooks[i]->RegisterHook(callback);
				return 1;
			}
		}

		return 0;
	}

	inline void CreateHook(NativeHashes type)
	{
		baseHooks[numBaseHooks++] = new BaseHook(type);
		baseHooks[numBaseHooks - 1]->Activate();
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
