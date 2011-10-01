////////////////////////////////////////////////////////////////
//		File name:  HookObjects.h
//		Part of:    Native-Hooks
//		Author:		thaCURSEDpie
//		Date:		September 2011
//
//		Description:
//		This header file describes the BaseHook class, which
//		each handle hooking 1 (one) native function.
//
////////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>
#include <Windows.h>

#include "HookSetup.h"
#include "Scripting.h"
#include "..\ScriptHook\ScriptingTypes.h"

namespace NativeHooks
{
	//
	// This class is unused at the moment.
	// It's intended use was for communication with the direct native hook and registered native by users.
	//
	class HookParams
	{
	protected:
		int* ints;
		float* floats;
		void** peds;
		void** vehicles;
		u32* unknowns;

		char* paramTypes;

	public:
		HookParams(u32 numInt, u32 numFloat, u32 numPed, u32 numVeh, u32 numUnk,
				   int* i_ints, float* i_floats, void** i_peds, void** i_vehs, u32* i_unknowns, char* params)
		{
			this->ints = new int[numInt];
			this->floats = new float[numFloat];
			this->peds = new void*[numPed];
			this->vehicles = new void*[numVeh];
			this->unknowns = new u32[numUnk];

			this->ints = i_ints;
			this->floats = i_floats;
			this->peds = i_peds;
			this->vehicles = i_vehs;
			this->unknowns = i_unknowns;

			this->paramTypes = params;
		}

		int* Ints()
		{
			return this->ints;
		}

		float* Floats()
		{
			return this->floats;
		}

		void** Peds()
		{
			return this->peds;
		}

		void** Vehicles()
		{
			return this->vehicles;
		}

		u32* Unknowns()
		{
			return this->unknowns;
		}
	};


	////////////////////////////////////////////////////////////////
	//		Name:			BaseHook
	//		Description:
	//			A class which handles the hooking of 1 (one)
	//			native function
	////////////////////////////////////////////////////////////////
	class BaseHook
	{
	protected:
		Scripting::NativeHashes type;

		void* funcAddr;
		void* thisDetourAddr;
		void* thisAddr;

		REGISTERED_HOOK** regHooks;

		BYTE* oldBytes;
		BYTE* newBytes;
		DWORD oldProt;

		enum
		{
			patchSize = 11,
			newProt = PAGE_EXECUTE_READWRITE,
		};

		bool isActivated;		
		int numRegisteredHooks;

		void nativeIntermediate(NativeContextEditable* cxt)
		{
			Log::Debug("hooked native 0x%x", this->type);

			for (int i = 0; i < this->numRegisteredHooks; i++)
			{
				regHooks[i](cxt);
			}

			this->DeActivate();

			NativeCall call = (NativeCall)this->FunctionAddress();
			call(cxt);

			this->Activate();
		}

	public:

		////////////////////////////////
		//		Accessors
		////////////////////////////////
		Scripting::NativeHashes Type()
		{
			return this->type;
		}

		void* FunctionAddress()
		{
			return this->funcAddr;
		}

		bool IsActivated()
		{
			return this->isActivated;
		}
		
		////////////////////////////////
		//		Constructor
		////////////////////////////////
		BaseHook(Scripting::NativeHashes hash)
		{
			this->numRegisteredHooks = 0;
			this->regHooks = new REGISTERED_HOOK*[50];

			this->funcAddr = Game::GetNativeAddressFromHash(hash);
		
			this->isActivated = false;
			this->type = hash;
			this->oldBytes = new BYTE[patchSize];
			this->newBytes = new BYTE[patchSize];

			this->newBytes[0]= 0xE9;																					// JMP
			this->newBytes[1] = 0x90; this->newBytes[2] = 0x90; this->newBytes[3] = 0x90; this->newBytes[4] = 0x90;		// <offset>
			this->newBytes[5] = 0xC3;																					// RET
					
			// We get the pointer to the BaseHook::nativeIntermediate function:
			this->thisDetourAddr = NULL;

			void (BaseHook::*addr)() = (void (BaseHook::*)())&BaseHook::nativeIntermediate;
			
			void* tempVoid = NULL;

			// We get our address
			__asm
			{
					push eax;						// backup eax
					mov eax,addr;					// copy the address of myClass::test into eax
					mov tempVoid,eax;				// copy the value of eax into testFunc
					pop eax;						// restore eax
			}

			this->thisDetourAddr = tempVoid;

			// Our jump-patch
			BYTE patch[] = {0xB9,						// MOV ecx, <value>
							0x90, 0x90, 0x90, 0x90,		// <value>
							0xE9,						// JMP <offset>
							0x90, 0x90, 0x90, 0x90,		// <offset>
							0xC3};						// RET

			// We patch in the class instance
			void* myAddr = this;
			memcpy(&patch[1], &myAddr, 4);

			// We patch in the jump offset
			DWORD jumpOffset = (DWORD)this->thisDetourAddr - (DWORD)this->funcAddr - 10;
			memcpy(&patch[6], &jumpOffset, 4);
			
			// DWORD jumpSize = ((DWORD)(hookFunc) - (DWORD)this->funcAddr - 5);

			// get access
			BOOL prot = VirtualProtect(this->funcAddr, this->patchSize, this->newProt, &this->oldProt);

			// save old bytes
			memcpy(this->oldBytes, this->funcAddr, this->patchSize);

			// set jump size in patch
			// memcpy(&this->newBytes[1], &jumpSize, 4);				

			// restore protection
			VirtualProtect(this->funcAddr, this->patchSize, this->oldProt, NULL);

			// Our testing patch
			memcpy(this->newBytes, patch, this->patchSize);
		}

		////////////////////////////////
		//		Methods
		////////////////////////////////
		void Activate()
		{
			if (this->isActivated)
			{
				return;
			}

			// get access
			VirtualProtect(this->funcAddr, this->patchSize, this->newProt, NULL);
			
			// patch in hook
			memcpy(this->funcAddr, this->newBytes, this->patchSize);
			
			// restore protection
			VirtualProtect(this->funcAddr, this->patchSize, this->oldProt, NULL);

			this->isActivated = true;
		}

		void DeActivate()
		{
			if (!this->isActivated)
			{
				return;
			}

			// get access
			VirtualProtect(this->funcAddr, this->patchSize, this->newProt, NULL);

			// patch in hook
			memcpy(this->funcAddr, this->oldBytes, this->patchSize);

			// restore protection
			VirtualProtect(this->funcAddr, this->patchSize, this->oldProt, NULL);

			this->isActivated = false;
		}	

		void RegisterHook(REGISTERED_HOOK* fnPtr)
		{
			// TODO: add bounds checking
			this->regHooks[this->numRegisteredHooks++] = fnPtr;
		}
	};	
}
