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

void CreateCarHook(NativeContext* cxt);

CustomThread::CustomThread()
{
    // Give your own name here!
	SetName("Native Hooks");
}

bool startup = true;

void CustomThread::RunTick()
{
	if ((GetAsyncKeyState(VK_F4) & 1) != 0)
    {
		u32 playerID = GetPlayerId();
		Ped playerChar;
		GetPlayerChar(playerID, &playerChar);

		SetCharHealth(playerChar, 100);
		PrintTxt("STRING", "Set Char HP set to 100", 1800, true);
    }

	if ((GetAsyncKeyState(VK_F5) & 1) != 0)
    {
		u32 playerID = GetPlayerId();
		Ped playerChar;
		GetPlayerChar(playerID, &playerChar);

		SetCharHealth(playerChar, 150);
		PrintTxt("STRING", "Set Char HP set to 150", 1800, true);
    }

	if ((GetAsyncKeyState(VK_F6) & 1) != 0)
    {
		u32 playerID = GetPlayerId();
		Ped playerChar;
		GetPlayerChar(playerID, &playerChar);

		SetCharHealth(playerChar, 200);
		PrintTxt("STRING", "Set Char HP set to 200", 1800, true);
    }

	// Tick
	if (startup)
	{
		Log::Debug(" startup\n");
		startup = false;	
		
		// Displaying some info:
		Log::Debug("NATIVE_CREATE_CAR  @ 0x%x", Game::GetNativeAddressFromHash(NATIVE_CREATE_CAR));
		Log::Debug("NATIVE_CREATE_CHAR @ 0x%x", Game::GetNativeAddressFromHash(NATIVE_CREATE_CHAR));
		Log::Debug("NATIVE_CREATE_OBJECT @ 0x%x", Game::GetNativeAddressFromHash(NATIVE_CREATE_OBJECT));
		Log::Debug("NATIVE_SET_CHAR_HEALTH @ 0x%x", Game::GetNativeAddressFromHash(NATIVE_SET_CHAR_HEALTH));
	    CreateBaseHooks();	

		RegisterHook(NATIVE_CREATE_CAR, CreateCarHook);
	}
}

void CreateCarHook(NativeContext* cxt)
{
	PrintTxt("STRING", "HOOKED :)", 1800, true);
}
