#include <iostream>
#include <fstream>
#include <Windows.h>
#include <winuser.h>
#include <memoryapi.h>
#include <string>
#include "methods.hpp"
#include "MinHook.h"

// make an installer for this that adds MinHook.x86.dll to the GeometryDash folder ok? ok ok

const char* LogFile = "D:\\SteamLibrary\\steamapps\\common\\Geometry Dash\\__GDLC.log";
const char* AppName = "GDLiveCollab";

// stolen directly from our good friend adafacafakaec's gdlivehook (thanks fam!)
DWORD base = (DWORD)GetModuleHandleA(0);
HMODULE libcocosbase = GetModuleHandleA("libcocos2d.dll");

// define return type and parameters for our trampoline function
typedef void (__fastcall *GDFUNC)(void*, void*, void*);

// trampoline function
GDFUNC fpAddObject = NULL;

struct EditLayer {
    BYTE pad[0x234];
    void* objects;
};

struct GameManager {
    BYTE pad[0x168];
    EditLayer* editLayer;
};

GameManager* gamemanager = (GameManager*)(base + 0x3222D0);

// the name of the function cocos2d calls to add an object ( libcocos2d.cocos2d::CCArray::addObject )
const char* addObjectFunctionCall = "?addObject@CCArray@cocos2d@@QAEXPAVCCObject@2@@Z";

void __fastcall test(void* _this, void* edx, void* CCObject) {
    if (gamemanager->editLayer && edx == gamemanager->editLayer->objects)
        MessageBoxA(NULL, "swaggy", AppName, MB_OK);

    // IMPORTANT!!: call trampoline (aka rest of the function we hooked to)
    return fpAddObject(_this, edx, CCObject);
}

DWORD WINAPI mainMod(LPVOID lpParam) {
    /*
    
    // awesome function from hook.h by firecubez (dont touch it works by magic)
    add_trampoline(

        // magical function that gets the address of the function we want to hook to
        (BYTE*)GetProcAddress(libcocosbase, addObjectFunctionCall),

        // our function we want to hook
        (BYTE*)test,

        // extra bytes calculated using black magic (check addObject with cheat engine, calc how many bytes of push / epd / whatever instructions until >4, subtract 5)
        0x2

    );

    //*/
    
    // Initialize hooker
    MH_STATUS ini = MH_Initialize();
    if (ini != MH_OK) {
        MessageBoxA(NULL, "Unable to load! (Can't initialize MinHook)", AppName, MB_OK);
        return 0;
    }

    // function we want to hook
    LPVOID targ_func = (LPVOID)GetProcAddress(libcocosbase, addObjectFunctionCall);

    // create and enable hook
    MH_CreateHook(targ_func, (BYTE*)test, reinterpret_cast<LPVOID*>(&fpAddObject));
    MH_EnableHook(targ_func);

    MessageBoxA(NULL, std::to_string(base + 0x3222D0).c_str() , AppName, MB_OK);

    // show message box
    MessageBoxA(NULL, ("Succesfully loaded " + (std::string)AppName + "!").c_str(), AppName, MB_OK);

    return 1;
}

// main dll entry point thing (works with black magic tm)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
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