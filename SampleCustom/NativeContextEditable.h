////////////////////////////////////////////////////////////////
//		File name:  NativeContextEditable.h
//		Part of:    Native-Hooks
//		Author:		thaCURSEDpie
//		Date:		September 2011
//
//		Description:
//		This header file describes the 'NativeContextEditable'
//		class, a class which inherits GameTypes::scrNativeCallContext
//		and is used to alter the parameter of native functions
//		upon hooking.
//
////////////////////////////////////////////////////////////////
#pragma once

#include "../ScriptHook/NativeContext.h"
#include "../ScriptHook/GameTypes.h"

namespace NativeHooks
{
	////////////////////////////////////////////////////////////////
	//		Name:			NativeContextEditable
	//		Description:
	//			A class which stores parameters passed to native
	//			functions and which can be used to view and alter
	//			the parameters.
	//			
	//			This class should **NOT** be initialized by your
	//			own code, but should be used as a parameter to
	//			a native-function-hook.
	//			Example:
	//
	//			void Foo(NativeContextEditable cxt)
	//			{
	//				// Do stuff...
	//			}
	//
	//			Use with caution please :).
	////////////////////////////////////////////////////////////////
	class NativeContextEditable : public GameTypes::scrNativeCallContext
	{
	private:
		enum
		{
			// This should be the same size as NativeContext::ArgSize !!
			argSize = sizeof(u32),
		};

	public:

		////////////////////////////////////////////////////////////////
		//		Constructor
		//			(you should never
		//			have to call this)
		////////////////////////////////////////////////////////////////
		__forceinline NativeContextEditable()
		{		
		}
		
		////////////////////////////////////////////////////////////////
		//		SetArgValue
		//		
		//		Is very likely the cause of crashes. Beware!
		//
		//			Params:
		//		T:			type of your variable
		//		argIndex:	index of the argument to change
		//			in the argument list. 0 would be the first (left-most) argument
		//		value:		value to pass
		//
		//			Returns:
		//		0:			Success
		//		1:			Argument type too large
		////////////////////////////////////////////////////////////////
		template <typename T>
		__declspec(deprecated("This function is very likely the cause of crashes. Beware! Do NOT use this function in it's current state for anything other than testing purposes.")) 
		__forceinline int SetArgValue(u32 argIndex, T value)
		{
			if (sizeof(T) > this->argSize)
			{
				// Error code 1: argument size too large
				return 1;
			}			
			else if (sizeof(T) < this->argSize)
			{
				// Create byte stack
				u8* stack = (u8*)this->m_pArgs;

				// First patch in padding
				u32 padding = 0;
				memcpy(&stack[argIndex], &padding, this->argSize);

				// Patch in new value
				memcpy(&stack[argIndex], &value, sizeof(T));
				return 0;
			}

			return 0;
		}
		

		////////////////////////////////////////////////////////////////
		//		GetArgValue
		//		
		//			Params:
		//		T:			type of your variable
		//		argIndex:	index of the argument to view
		//			in the argument list. 0 would be the first (left-most) argument
		//		ret:		a value to store the argument in
		//
		//			Returns:
		//		0:			Success
		//		1:			Invalid argument number
		//		2:			Argument type too large
		////////////////////////////////////////////////////////////////
		template <typename T>
		__forceinline int GetArgValue(u32 argIndex, T* ret)
		{
			if (argIndex >= this->m_nArgCount)
			{
				// Error code 1: invalid argument number
				return 1;
			}

			if (sizeof(T) > this->argSize)
			{
				// Error code 2: type is too large
				return 2;
			}	
			else if (sizeof(T) == this->argSize)
			{		
				T* stack = (T*)this->m_pArgs;

				*ret = stack[argIndex];

				return 0;
			}
			else if (sizeof(T) < this->argSize)
			{
				// We are too small
				T tempT;

				u8* stack = (u8*)this->m_pArgs;

				memcpy(&tempT, &stack[argIndex], sizeof(T));

				*ret = tempT;

				return 0;
			}
		}

		__forceinline size_t GetArgSize()
		{
			return this->argSize;
		}

		__forceinline u32 GetArgCount()
		{
			return this->m_nArgCount;
		}

		__forceinline u32 GetDataCount()
		{
			return this->m_nDataCount;
		}
	};
}
