#pragma once

#include "../ScriptHook/ScriptThread.h"
#include "../ScriptHook/ScriptingEnums.h"

class CustomThread : public ScriptThread
{
private:

protected:
    // We want a Tick based script, so we override RunTick.
	void RunTick();

public:
	CustomThread();
};

int CreateCarCallback(int* carHash, float* X, float* Y, float* Z, int** handle , bool* unk, int previousChanges);
