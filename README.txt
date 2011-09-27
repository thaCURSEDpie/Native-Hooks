NOTE: PROJECT IS VERY MUCH WORK IN PROGRESS (VERY MUCH PRE-ALPHA. NOT FIT FOR USE!)
    (but it hooks :)
    
Project name:   Native-Hooks
Author:         thaCURSEDpie
                    (Lots of help from MulleDK19!)
Date:           September 2011
Type:           game modification (GTA:IV)
Language:       C++
Description:
    A mod for GTA allowing scripters to hook native functions.
    
Description of files:
    HookObjects.h:
        Holds the BaseHook class, which handles hooking/unhooking.
        
    HookSetup.h:
        Header file for HookSetup.cpp.
        
    NativeContextEditable.h:
        Contains the NativeContextEditable class, which allows the viewing and changing of native-functions parameters.
        
    Script.h:
        Header file for Script.cpp.
        
    Scripting.h:
        Can hold additional natives.
        
    HookSetup.cpp:
        Manages all the hooking. This file is actually kind of a class on it's own, but it's not a class because of restrictions when hooking functions (in the end it all comes down to the fact that you can't convert pointer to member functions to regular function pointers using C++. There is a way to do this using assembly but this is not implemented yet.)
        
    Main.cpp:
        DLL main.
        
    Script.cpp:
        Commands the hooks to be initiated.
        
More info:
    The current state is pre-alpha: there is still some testing code, I haven't been able to get things working the way I wanted them to.
  
    An explaination as to how things (and hooking in general) are done:
    Native-Functions, are, like the name says functions. This allows one to hook them, like one is able to hook pretty much any function. This hooking is done by changing the opcodes at the start of the function to reroute the flow of code to one's own function.
    
    The patch is as follows:
    
    BYTE patch[] = {0xE9,			         // JMP
                    0x90, 0x90, 0x90, 0x90,	 // <offset>
                    0xC3};                   // RET
                    
        Note that this is Intel x86. At the moment this is the only architecture supported. (though I don't know if GTA:IV even supports other architectures.)
    
    The offset is calculated by getting the function-address (using a function provided by Aru's C++ Scripthook).
    The hooking and un-hooking is all handled by the 'BaseHook' class (see HookObjects.h)
    
    The problem with hooking is that you have to get the offset to your detour-function. This is not possible with class functions. In the end it all comes down to the fact that you can't convert pointer to member functions to regular function pointers using C++. There is a way to do this using assembly but this is not implemented yet.
    
    Because of this there isn't yet a nice API which allows users to register native functions. As such, the project is not usable yet.
    
Compiling:
    Create a new project in MS Visual Studio, add the source & header files, compile away. (Depends on scripthook.dll. Don't ask me how to configure MS VS, I haven't got a clue.)