#pragma once

#include <Windows.h>
#include "Scripting.h"

namespace NativeHooks
{
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
			this->detourAddr = hookFunc;
			
			this->isActivated = false;
			this->type = hash;
			this->oldBytes = new BYTE[patchSize];
			this->newBytes = new BYTE[patchSize];

			this->newBytes[0]= 0xE9;																					// PUSH
			this->newBytes[1] = 0x90; this->newBytes[2] = 0x90; this->newBytes[3] = 0x90; this->newBytes[4] = 0x90;		// <offset>
			this->newBytes[5] = 0xC3;																					// RET
							  

			DWORD jumpSize = ((DWORD)hookFunc - (DWORD)this->funcAddr - 5);

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
