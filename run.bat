@echo off

set INP_NAME=dllmain.cpp
set DLL_NAME=GDLiveCollab.dll
set EXE_NAME=GDLiveCollab.exe
set COM_PARM=-luser32 -lDbghelp -llibMinHook-x86-v141-mt
set RUN_ARGS=""
set TES_MODE=0
set DLL_COMP=1

FOR %%A IN (%*) DO (
    IF "%%A"=="-a" set RUN_ARGS=""
    IF "%%A"=="-t" set TES_MODE=1
    IF "%%A"=="-r" set DLL_COMP=0
)

if %DLL_COMP%==1 (
    echo Compiling DLL...
    clang++ -std=c++17 -m32 -shared %COM_PARM% -o %DLL_NAME% %INP_NAME%
    if %errorlevel% == 0 (echo Compiled DLL!) else (goto error)
)
if %TES_MODE%==1 (
    echo Running...
    clang++ test.cpp -o test.exe
    if %errorlevel%==0 (test.exe) else (goto error)
) else (
    echo Compiling Runner...
    clang++ runner.cpp -o %EXE_NAME% -std=c++17 -m32 -luser32
    if %errorlevel%==0 (
        echo Compiled Runner!
        echo.
        %EXE_NAME%
    ) else (goto error)
)
goto done

:error
echo Compile error, see output above
goto done

:done