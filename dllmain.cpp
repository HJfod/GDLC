#include <iostream>
#include <Windows.h>
#include <winuser.h>
#include <memoryapi.h>
#include "hook.h"

const char* AppName = "GDLiveBuild";

// stolen directly from our good friend adafacafakaec's gdlivehook (thanks fam!)
DWORD base = (DWORD)GetModuleHandleA(0);
HMODULE libcocosbase = GetModuleHandleA("libcocos2d.dll");

// the name of the function cocos2d calls to add an object
const char* addObjectFunctionCall = "?addObject@CCArray@cocos2d@@QAEXPAVCCObject@2@@Z";

// 0x66382490 appears to be the correct function ( libcocos2d.cocos2d::CCArray::addObject )

void __cdecl test() {
    MessageBoxA(NULL, "God damn it works", AppName, MB_OK);
}

DWORD WINAPI mainMod(LPVOID lpParam) {
    // awesome function from hook.h by firecubez (dont touch it works by magic)
    add_trampoline(
        // magical function that gets the address of the function we want to hook to
        (BYTE*)GetProcAddress(libcocosbase, addObjectFunctionCall),
        // our function we want to hook
        (BYTE*)test,
        // extra bytes calculated using black magic (check addObject with cheat engine, calc how many bytes of push / epd / whatever instructions until >4, subtract 5)
        0x2
    );

    // show a message box
    MessageBoxA(NULL, "Succesfully loaded!", AppName, MB_OK);

    return 1;
}

// main dll entry point thing (works with black magic tm)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
		    CreateThread(0, 0x1000, &mainMod, 0, 0, NULL);
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
	}
	return TRUE;
}