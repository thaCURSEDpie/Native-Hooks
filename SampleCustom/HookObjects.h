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

		void* detourAddr;
		void* funcAddr;
		void** callbacks;
		void** postHooks;

		BYTE* oldBytes;
		BYTE* newBytes;
		DWORD oldProt;

		enum
		{
			patchSize = 6,
			newProt = PAGE_EXECUTE_READWRITE,
		};

		bool isActivated;		
		int numCallbacks;
		int numPosthooks;	

		void nativeIntermediate(GameTypes::scrNativeCallContext* cxt)
		{
			Log::Info("hooked native %u", this->type);

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

		void* DetourAddress()
		{
			return this->detourAddr;
		}

		void* FunctionAddress()
		{
			return this->funcAddr;
		}

		void** CallbackFunctions()
		{
			return this->callbacks;
		}

		void** PostHooks()
		{
			return this->postHooks;
		}

		int GetNumCallbacks()
		{
			return this->numCallbacks;
		}

		int GetNumPostHooks()
		{
			return this->numPosthooks;
		}

		bool IsActivated()
		{
			return this->isActivated;
		}
		
		////////////////////////////////
		//		Constructor
		////////////////////////////////
		BaseHook(Scripting::NativeHashes hash, void* hookFunc)
		{
			this->callbacks = new void*[50];

			this->funcAddr = Game::GetNativeAddressFromHash(hash);

			// char* str = new char[100];
			// sprintf(str, "0x%x", this->funcAddr);
			// MessageBoxA(NULL, str, "Native address:", 0);

			this->detourAddr = hookFunc;
			
			this->isActivated = false;
			this->type = hash;
			this->oldBytes = new BYTE[patchSize];
			this->newBytes = new BYTE[patchSize];

			this->newBytes[0]= 0xE9;																					// JMP
			this->newBytes[1] = 0x90; this->newBytes[2] = 0x90; this->newBytes[3] = 0x90; this->newBytes[4] = 0x90;		// <offset>
			this->newBytes[5] = 0xC3;																					// RET
							  

			//////////////
			// UNUSED
			//////////////
			// Our patch
			//  We store the native type in a global variable (see HookSetup.cpp),
			//  so we know what native called our hook function.
			/*
			BYTE patch[] = {0xC7, 0x05,			         // MOV [target], <data>
							0x90, 0x90, 0x90, 0x90,		 // [target]
							0x90, 0x90, 0x90, 0x90,		 // <data>
							0xE9,					     // JMP
							0x90, 0x90, 0x90, 0x90,	     // <offset>
							0xC3};					     // RET
			
			// We patch in the address where we will store the native-type
			u32* paramAddr = getParamAddress();
			memcpy(&patch[2], &paramAddr, 4);

			// We patch in the native type to store in the address
			u32 nativeType = (u32)this->type;
			memcpy(&patch[6], &nativeType, 4);

			// We patch in the jump offset
			DWORD jumpOffset = (DWORD)hookFunc - (DWORD)this->funcAddr - 15;
			memcpy(&patch[11], &jumpOffset, 4);
			

			BYTE patch[] = {0xB8,				         // MOV EAX, <data>
							0x90, 0x90, 0x90, 0x90,		 // <data>
							0xE9,					     // JMP
							0x90, 0x90, 0x90, 0x90,	     // <offset>
							0xC3};					     // RET

			// We patch in the native type to store in the address
			u32 nativeType = (u32)this->type;
			memcpy(&patch[1], &nativeType, 4);

			// We patch in the jump offset
			DWORD jumpOffset = (DWORD)hookFunc - (DWORD)this->funcAddr - 10;
			memcpy(&patch[6], &jumpOffset, 4);

			*/
			
			DWORD jumpSize = ((DWORD)(hookFunc) - (DWORD)this->funcAddr - 5);

			// get access
			BOOL prot = VirtualProtect(this->funcAddr, this->patchSize, this->newProt, &this->oldProt);

			// save old bytes
			memcpy(this->oldBytes, this->funcAddr, this->patchSize);

			// set jump size in patch
			memcpy(&this->newBytes[1], &jumpSize, 4);				

			// restore protection
			VirtualProtect(this->funcAddr, this->patchSize, this->oldProt, NULL);
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

		void AddPostHook(void* function)
		{
			int arraySize = sizeof(this->postHooks) / sizeof(void*);

			// We have enough room
			if (numPosthooks < arraySize)
			{
				this->postHooks[this->numPosthooks++] = function;				
			}
			else
			{
				void** tmp = new void*[arraySize];
				tmp = this->callbacks;

				this->postHooks = new void*[(int)(arraySize * 1.5)];
				memcpy(this->callbacks, tmp, arraySize);

				this->postHooks[this->numPosthooks++] = function;
			}
		}

		void AddCallback(void* function)
		{
			int arraySize = sizeof(this->callbacks) / sizeof(void*);

			// We have enough room
			if (numCallbacks < arraySize)
			{
				callbacks[numCallbacks++] = function;				
			}
			else
			{
				void** tmp = new void*[arraySize];
				tmp = this->callbacks;

				callbacks = new void*[(int)(arraySize * 1.5)];
				memcpy(this->callbacks, tmp, arraySize);

				callbacks[numCallbacks++] = function;
			}
		}
	};	
}
