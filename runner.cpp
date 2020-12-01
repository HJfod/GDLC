#include <iostream>
#include <string>
#include <Windows.h>
#include <WinUser.h>
#include "methods.hpp"

const char* AppName = "GDLiveCollab";
const char* DataFile = "__GDLC";

enum err {
    PROG_SUCCESS = 0,
    FILE_NOT_FOUND,
    GD_PATH_NOT_VALID
};

const char* req_files[2] = {
    "GDLiveCollab.dll",
    "MinHook.x86.dll"
};

int throwErr(std::string _msg, int _err) {
    std::cout << " Failed" << std::endl;
    MessageBoxA(NULL, _msg.c_str(), AppName, MB_OK);

    exit(_err);
}

int main(int argc, char* argv[]) {
    std::string GDDataPath = "";

    if (methods::fexists(DataFile))
        GDDataPath = methods::fread(DataFile);
    else {
        std::cout << "Type the path to GeometryDash.exe" << std::endl;
        getline(std::cin, GDDataPath);
    }

    std::cout << "Checking data path...";

    bool gddpcs = false;
    if (methods::ewith(GDDataPath, "GeometryDash.exe"))
        if (methods::fexists(GDDataPath))
            gddpcs = true;
    if (!gddpcs)
        throwErr("GeometryDash.exe path does not appear to be correct.", GD_PATH_NOT_VALID);
    
    methods::fsave(DataFile, GDDataPath);
    std::cout << " Success" << std::endl;

    std::cout << "Checking required files...";

    for (std::string f : req_files)
        if (!methods::fexists(f))
            throwErr("Unable to load: " + f + " not found! (Did you forget to unzip?)", FILE_NOT_FOUND);
    
    std::cout << " Success" << std::endl;

    return PROG_SUCCESS;
}