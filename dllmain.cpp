#include <iostream>
#include <fstream>
#include <Windows.h>
#include <winuser.h>
#include <memoryapi.h>
#include <string>
#include "methods.hpp"
#include "MinHook.h"

// TODO:
// make an installer for this that adds MinHook.x86.dll to the GeometryDash folder ok? ok ok

const char* LogFile = "D:\\SteamLibrary\\steamapps\\common\\Geometry Dash\\__GDLC.log";
const char* AppName = "GDLiveCollab";

// stolen directly from our good friend adafacafakaec's gdlivehook (thanks fam!)
// gets the offset of GeometryDash.exe
DWORD base = (DWORD)GetModuleHandleA(0);

// define return type and parameters for our trampoline function
typedef void (__fastcall *GDFUNC)(void*, void*, void*);

// trampoline function
GDFUNC fpAddObject = NULL;

/*
 * 
 * some functions:
 * 
 * base + 0x85680       > called by gd when an object is placed / edited (dont know what it is though)
 * base + 0x85750       > called by gd when an object is placed (this is what im looking for!! thanks shira!!)
 * 
 */

// the function GD calls to add an object (  )
DWORD addObjectFunctionCall = base + 0x85750;

void __fastcall test(void* _this, void* edx, void* CCObject) {
    MessageBoxA(NULL, "swaggy", AppName, MB_OK);

    // IMPORTANT!!: call trampoline (aka rest of the function we hooked to)
    return fpAddObject(_this, edx, CCObject);
}

DWORD WINAPI mainMod(LPVOID lpParam) {
    // Initialize hooker
    MH_STATUS ini = MH_Initialize();
    if (ini != MH_OK) {
        MessageBoxA(NULL, "Unable to load! (Can't initialize MinHook)", AppName, MB_OK);
        return 0;
    }

    // function we want to hook
    LPVOID targ_func = (LPVOID)addObjectFunctionCall;

    // create and enable hook
    MH_CreateHook(targ_func, (BYTE*)test, reinterpret_cast<LPVOID*>(&fpAddObject));
    MH_EnableHook(targ_func);

    // show message box
    MessageBoxA(NULL, ("Succesfully loaded " + (std::string)AppName + "!").c_str(), AppName, MB_OK);

    return 1;
}

// main dll entry point thing
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // begin hooking when dll attached
		    CreateThread(0, 0x1000, &mainMod, 0, 0, NULL);
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            // properly handle closing
            MH_Uninitialize();
            // MessageBoxA(NULL, ("Removed " + (std::string)AppName + "! :)").c_str(), AppName, MB_OK);
            break;
	}
	return TRUE;
}