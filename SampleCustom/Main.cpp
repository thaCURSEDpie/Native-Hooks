////////////////////////////////////////////////////////////////
//		File name:  Main.cpp
//		Part of:    Native-Hooks
//		Author:		Aru
//		Date:		Unknown
//
//		Description:
//		This is the DLL main, which registers the custom thread
//		when the module is loaded by GTA.
//
////////////////////////////////////////////////////////////////

#include <windows.h>

#include "Script.h"


#include "../ScriptHook/ScriptHookManager.h"
#include "../ScriptHook/Log.h"

static CustomThread _customThread;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: 
        {
            ScriptHookManager::RegisterThread( &_customThread );
			
            break;
        }
    case DLL_PROCESS_DETACH:
        {
            break;
        }
    }

    return TRUE;
}