#include <iostream>
#include <string>
#include <Windows.h>
#include <WinUser.h>
#include "methods.hpp"
#include "WtsApi32.h"
#include <ctype.h>
#include <tlhelp32.h>
#include <Shlwapi.h>

const char* AppName = "GDLiveCollab";
const char* DataFile = "__GDLC";
bool NoMsgBox = false;
DWORD GD_PID = 0;

#define INJECT_SUCCESS 0x3F
#define INJECT_TARGET_OPEN_FAIL 0x30
#define INJECT_TARGET_MALLOC_FAIL 0x31
#define INJECT_TARGET_CANT_WRITE 0x32
#define INJECT_TARGET_CANT_CREATE_THREAD 0x33
#define INJECT_ERROR_UNKNOWN 0x34

enum err {
    PROG_SUCCESS = 0,
    FILE_NOT_FOUND,
    GD_PATH_NOT_VALID,
    INJECT_ERROR
};

const char* req_files[2] = {
    "GDLiveCollab.dll",
    "MinHook.x86.dll"
};

int throwErr(std::string _msg, int _err) {
    std::cout << " Failed" << std::endl;

    if (!NoMsgBox)
        MessageBoxA(NULL, _msg.c_str(), AppName, MB_OK);

    exit(_err);
}

bool proc_running(const char* _proc, DWORD* _pid = NULL) {
    WTS_PROCESS_INFO* pWPIs = NULL;
    DWORD dwProcCount = 0;
    bool found = false;
    if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
        for (DWORD i = 0; i < dwProcCount; i++)
            if (strcmp((LPSTR)pWPIs[i].pProcessName, _proc) == 0) {
                found = true;
                if (_pid != NULL)
                    *_pid = pWPIs[i].ProcessId;
            }

    if (pWPIs) {
        WTSFreeMemory(pWPIs);
        pWPIs = NULL;
    }

    return found;
}

int InjectDLL(const int &pid, const std::string &DLL_Path) {
    // adapted from https://github.com/saeedirha/DLL-Injector
    
    long dll_size = DLL_Path.length() + 1;
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (hProc == NULL)
        return INJECT_TARGET_OPEN_FAIL;

    LPVOID MyAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (MyAlloc == NULL)
        return INJECT_TARGET_MALLOC_FAIL;

    int IsWriteOK = WriteProcessMemory(hProc , MyAlloc, DLL_Path.c_str() , dll_size, 0);
    if (IsWriteOK == 0)
        return INJECT_TARGET_CANT_WRITE;

    DWORD dWord;
    LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryA");
    HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, MyAlloc, 0, &dWord);
    if (ThreadReturn == NULL)
        return INJECT_TARGET_CANT_CREATE_THREAD;

    if ((hProc != NULL) && (MyAlloc != NULL) && (IsWriteOK != ERROR_INVALID_HANDLE) && (ThreadReturn != NULL))
        return INJECT_SUCCESS;

    return INJECT_ERROR_UNKNOWN;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "mb") == 0)
        NoMsgBox = true;
    
    std::cout << "MsgBox: " << NoMsgBox << std::endl;

    std::string GDDataPath = "";

    if (methods::fexists(DataFile))
        GDDataPath = methods::fread(DataFile);
    else {
        std::cout << "Type the path to GeometryDash.exe" << std::endl;
        getline(std::cin, GDDataPath);
    }

    ////////////////////////////

    std::cout << "Checking data path...";

    bool gddpcs = false;
    if (methods::ewith(GDDataPath, "GeometryDash.exe"))
        if (methods::fexists(GDDataPath))
            gddpcs = true;
    if (!gddpcs)
        throwErr("GeometryDash.exe path does not appear to be correct.", err::GD_PATH_NOT_VALID);

    GDDataPath = methods::replace(GDDataPath, "/", "\\");
    methods::fsave(DataFile, GDDataPath);

    std::cout << " Success" << std::endl;

    ////////////////////////////

    std::cout << "Checking required files...";

    for (std::string f : req_files)
        if (!methods::fexists(f))
            throwErr("Unable to load: " + f + " not found! (Did you forget to unzip?)", err::FILE_NOT_FOUND);
    
    std::cout << " Success" << std::endl;
    
    ////////////////////////////

    std::cout << "Checking GD status...";

    if (!proc_running("GeometryDash.exe", &GD_PID))
        throwErr("GD isn't running! GD needs to be open to load this program.", err::FILE_NOT_FOUND);

    std::cout << " Success " << "(PID: " << GD_PID << ")" << std::endl;
    
    ////////////////////////////

    std::cout << "Moving files...";

    std::string nMHpath = GDDataPath.substr(0, GDDataPath.find_last_of("\\") + 1) + req_files[1];
    if (!methods::fexists(nMHpath))
        if (methods::fcopy(req_files[1], nMHpath) != METH_SUCCESS)
            throwErr("There was an error copying files (METH_COPY_FROM_DOESNT_EXIST, probably)", err::FILE_NOT_FOUND);
    
    std::cout << " Success" << std::endl;
    
    ////////////////////////////

    std::cout << "Injecting DLL...";

    int dll_suc = InjectDLL(GD_PID, methods::workdir() + "\\" + req_files[0]);
    if (dll_suc != INJECT_SUCCESS)
        throwErr("Unable to inject DLL! (Error code: " + std::to_string(dll_suc) + ")", err::INJECT_ERROR);
    
    std::cout << " Success" << std::endl;
    
    ////////////////////////////

    MessageBoxA(NULL, "Succesfully loaded! :)", AppName, MB_OK);

    return PROG_SUCCESS;
}