////////////////////////////////////////////////////////////////
//		File name:  Script.cpp
//		Part of:    Native-Hooks
//		Author:		thaCURSEDpie
//		Date:		September 2011
//
//		Description:
//		This source file contains the script part of the project.
//		The script is used to initialize the native hooks
//		(which is done by calling the 'CreateBaseHooks' function)
//
////////////////////////////////////////////////////////////////

#include "Script.h"

#include "HookSetup.h"
#include "Scripting.h"
#include "../ScriptHook/Log.h"

#include <windows.h>

#define PrintTxt PrintStringWithLiteralStringNow

// Pull in all our scripting functions/types
using namespace Scripting;
using namespace NativeHooks;

void CreateCarHook(NativeContextEditable* cxt);

CustomThread::CustomThread()
{
    // Give your own name here!
	SetName("Native Hooks");
}

bool startup = true;

void CustomThread::RunTick()
{
	// Tick
	if (startup)
	{
		Log::Debug(" startup\n");
		startup = false;	
		
		CreateBaseHooks();	

		RegisterHook(NATIVE_CREATE_CAR, CreateCarHook);
	}
}

void CreateCarHook(NativeContextEditable* cxt)
{
	PrintTxt("STRING", "HOOKED :)", 1800, true);
}
