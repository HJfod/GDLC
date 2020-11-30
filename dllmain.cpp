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
int increment = 0;

// stolen directly from our good friend adafacafakaec's gdlivehook (thanks fam!)
DWORD base = (DWORD)GetModuleHandleA(0);
HMODULE libcocosbase = GetModuleHandleA("libcocos2d.dll");

struct EditLayer {
    BYTE pad[0x234];
    void* objects;
};

struct GameManager {
    BYTE pad[0x168];
    EditLayer* editLayer;
};

GameManager* gamemanager = (GameManager*)(base + 0x3222D0);

// the name of the function cocos2d calls to add an object
const char* addObjectFunctionCall = "?addObject@CCArray@cocos2d@@QAEXPAVCCObject@2@@Z";

// 0x66382490 appears to be the correct function ( libcocos2d.cocos2d::CCArray::addObject )

void __fastcall test(void* _this, void* edx, void* CCObject) {
    try {
        _this;
        MessageBoxA(NULL, "awesome sauce", AppName, MB_OK);

        // after 20 message boxes it crashes, best guess is _this is not defined then ?
        
        // if (gamemanager->editLayer && _this == gamemanager->editLayer->objects)
    } catch (const std::exception& e) {}
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
    
    MH_STATUS ini = MH_Initialize();
    if (ini != MH_OK) {
        MessageBoxA(NULL, "Unable to load! (Can't initialize MinHook)", AppName, MB_OK);
        return 0;
    }

    BYTE* targ_func = (BYTE*)GetProcAddress(libcocosbase, addObjectFunctionCall);

    MH_STATUS hook = MH_CreateHook(targ_func, (BYTE*)test, NULL);
    MH_STATUS ena = MH_EnableHook(targ_func);

    // show a message box
    MessageBoxA(NULL, "Succesfully loaded!", AppName, MB_OK);

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
            MH_Uninitialize();
            MessageBoxA(NULL, "Removed DLL", AppName, MB_OK);
            break;
	}
	return TRUE;
}