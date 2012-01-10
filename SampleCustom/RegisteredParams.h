#pragma once

#include <Windows.h>
#include "Scripting.h"
#include "..\ScriptHook\ScriptingTypes.h"

using namespace Scripting;

class RegisteredParams
{
private:
	BYTE* bytes;
	int argc;
	NativeHashes type;

public:
	RegisteredParams(NativeHashes type, int argc, size_t numBytes, BYTE* params)
	{
		this->type = type;
		this->bytes = new BYTE[numBytes];
			
		memcpy(this->bytes, params, numBytes);

		this->argc = argc;
	}

	~RegisteredParams()
	{
		delete this->bytes;
	}

	template <typename T>
	T GetParamValue(u32 index)
	{
		T retVal;
			
		if (sizeof(T) != sizeof(u32) ||
			index >= (u32)argc)
		{
			return retVal;
		}

		memcpy(&retVal, &this->bytes[index], sizeof(u32));

		return retVal;
	}

	NativeHashes Type()
	{
		return this->type;
	}

	int GetArgCount()
	{
		return this->argc;
	}
};
