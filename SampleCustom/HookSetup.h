////////////////////////////////////////////////////////////////
//		File name:  HookSetup.h
//		Part of:    Native-Hooks
//		Author:		thaCURSEDpie
//		Date:		September 2011
//
//		Description:
//		This is the header file for the 'HookSetup.cpp' source
//		file.
//
////////////////////////////////////////////////////////////////
#pragma once

#include "Scripting.h"
#include "../ScriptHook/Log.h"
#include "NativeContextEditable.h"

#include <windows.h>

namespace NativeHooks
{
	typedef void (_cdecl *NativeCall)(GameTypes::scrNativeCallContext *cxt);

	////////////////////////////////////////////////////////////////
	//		Native callback prototypes
	//			note:	these have one parameter more than the
	//					regular native functions, this param is used
	//					to communicate if the arguments have been
	//					changed already by another hook.
	//
	//					0:	No change
	//					1:  Argument(s) have been changed
	//					2:  Function is being blocked
	////////////////////////////////////////////////////////////////
	typedef int (*NCB_CREATE_CAR)(int*, float*, float*, float*, int**, bool*, int);

	typedef void (REGISTERED_HOOK)(NativeContext *cxt); //Editable *cxt);

	////////////////////////////////////////////////////////////////
	//		Native intermediates
	//			this is where the natives are rerouted to,
	//			they handle the client callbacks
	////////////////////////////////////////////////////////////////
	void NI_CREATE_CAR(NativeContextEditable *cxt);
	void NI_GENERAL(GameTypes::scrNativeCallContext *cxt);


	////////////////////////////////////////////////////////////////
	//		Hook requests
	//			allow you to add your hooks :)
	////////////////////////////////////////////////////////////////
	int PreHook_CreateCar(NCB_CREATE_CAR cbFn);
	int PostHook_CreateCar(NCB_CREATE_CAR cbFn);

	////////////////////////////////////////////////////////////////
	//		Misc.
	////////////////////////////////////////////////////////////////

	// Initiate Native-Hooks
	void CreateBaseHooks();

	////////////////////////////////////////////////////////////////
	//		getBaseHookIndex
	//			gets the index for a certain base hook
	//
	//			Params:
	//		type:		native type
	//
	//			Returns:
	//		-1:					failure. Hook does not exist
	//								(this native is unsupported)
	//		any other value:	success
	////////////////////////////////////////////////////////////////
	int getBaseHookIndex(Scripting::NativeHashes type);	

	////////////////////////////////////////////////////////////////
	//		IsHookEnabled
	//			checks if a hook is enabled
	//
	//			Params:
	//		type:		native type
	//
	//			Returns:
	//		a bool representing if the hook is enabled or not
	////////////////////////////////////////////////////////////////
	bool IsHookEnabled(Scripting::NativeHashes type);


	int RegisterHook(Scripting::NativeHashes type, REGISTERED_HOOK* callback);

	void CreateHook(Scripting::NativeHashes type);
}
