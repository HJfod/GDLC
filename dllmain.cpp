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
DWORD libcocosbase = (DWORD)GetModuleHandleA("libcocos2d.dll");

// define return type and parameters for our trampoline function
typedef void (__fastcall *GDFUNC)(void*, void*, void*);
typedef void(__stdcall* fPasteFunction)(std::string testString);

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
fPasteFunction pasteFunction = (fPasteFunction)(base + 0x88240);

struct CCObject {
    const char* str = "";
    int id;
};

void placeJump(BYTE* address, DWORD jumpTo, DWORD length) {
	DWORD oldProtect, newProtect, relativeAddress;
	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtect);
	relativeAddress = (DWORD)(jumpTo - (DWORD)address) - 5;
	*address = 0xE9;
	*((DWORD*)(address + 0x1)) = relativeAddress;
	for (DWORD x = 0x5; x < length; x++)
		*(address + x) = 0x90;
	VirtualProtect(address, length, oldProtect, &newProtect);
}

DWORD createWithSprite = base + 0x282284;
DWORD createWithSprite_ = (DWORD)createWithSprite;
DWORD operatorPlus = base + 0x282278;
DWORD operatorPlus_ = (DWORD)operatorPlus;
DWORD sharedDirector = base + 0x282270;
DWORD sharedDirector_ = (DWORD)sharedDirector;

DWORD gameManager = base + 0x3222D0;
DWORD retPasteObjects = base + 0x88245;

void print(std::string msg) {
    std::cout << AppName << ": " << msg << std::endl;
}

__declspec(naked) void pasteObjects() {
	__asm {
		mov ecx, gameManager
		mov ecx, [ecx]
		add ecx, 0x168
		mov ecx, [ecx]
		test ecx, ecx
		je return_end
		add ecx, 0x380
		mov ecx, [ecx]
		test ecx, ecx
		je return_end
		push ebp
		mov ebp, esp
		push -0x01
		jmp[retPasteObjects]
		return_end:
		ret 0x0018
	}
}

__declspec(naked) void loadPointer() {
	__asm {
		pushad
		mov eax, createWithSprite_
		mov eax, [eax]
		mov[createWithSprite], eax
		mov eax, operatorPlus_
		mov eax, [eax]
		mov[operatorPlus], eax
		mov eax, sharedDirector_
		mov eax, [eax]
		mov[sharedDirector], eax
		popad
		ret

	}
}

void InsertObject(std::string _obj) {
    print(_obj);

    _obj += "\0";

	DWORD oldProtect, newProtect;

    print("debug0");
    
    VirtualProtect((LPVOID)(libcocosbase + 0xC16A3), 8, PAGE_EXECUTE_READWRITE, &oldProtect);
    *((__int64*)(libcocosbase + 0xC16A3)) = 0x0E74000000026DE9;

	std::this_thread::sleep_for(std::chrono::milliseconds(20));

    print("debug1");

    pasteFunction(_obj);

	std::this_thread::sleep_for(std::chrono::milliseconds(20));

    print("debug2");

    *((__int64*)(libcocosbase + 0xC16A3)) = 0x0E74000000958638;
    VirtualProtect((LPVOID)(libcocosbase + 0xC16A3), 8, oldProtect, &newProtect);

    print("debug3");
}

void __fastcall AddObjectHook(void* _this, void* _edx, void* _CCObject) {
    CCObject* CC = (CCObject*)_CCObject;
    //MessageBoxA(NULL, CC->str, AppName, MB_OK);

    InsertObject("1,1,2,30,3,50");

    // IMPORTANT!!: call trampoline (aka rest of the function we hooked to)
    return fpAddObject(_this, _edx, _CCObject);
}

DWORD WINAPI mainMod(LPVOID lpParam) {
    // Initialize hooker
    MH_STATUS ini = MH_Initialize();
    if (ini != MH_OK) {
        MessageBoxA(NULL, "Unable to load! (Can't initialize MinHook)", AppName, MB_OK);
        return 0;
    }

    // redirect console output
    if (AllocConsole() == 0) {
        MessageBoxA(NULL, "Unable to redirect console output!", AppName, MB_ICONERROR);
        return 0;
    }
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

    // Initialize stuff stolen from GDLiveCollab
    loadPointer();
	placeJump((BYTE*)base + 0x88240, (DWORD)pasteObjects, 0x5);

    // function we want to hook
    LPVOID targ_func = (LPVOID)addObjectFunctionCall;

    // create and enable hook
    MH_CreateHook(targ_func, (BYTE*)AddObjectHook, reinterpret_cast<LPVOID*>(&fpAddObject));
    MH_EnableHook(targ_func);

    // show message box
    //MessageBoxA(NULL, ("Succesfully loaded " + (std::string)AppName + "!").c_str(), AppName, MB_OK);

    // load succesful, probably
    print("Succesfully loaded!");

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