////////////////////////////////////////////////////////////////
//		File name:  Script.h
//		Part of:    Native-Hooks
//		Author:		Aru
//		Date:		Unknown
//
//		Description:
//		This is the header file for the Script.cpp source
//		file.
//
//		It contains the CustomThread class, which inherits
//		ScriptThread.
//
////////////////////////////////////////////////////////////////
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
