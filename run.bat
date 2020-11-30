@echo off

set INP_NAME=dllmain.cpp hook.cpp
set DLL_NAME=test.dll
set COM_PARM=-luser32 -lDbghelp
set RUN_ARGS=""
set TES_MODE=0

FOR %%A IN (%*) DO (
    IF "%%A"=="-a" set RUN_ARGS=""
    IF "%%A"=="-t" set TES_MODE=1
)

echo Compiling...
clang++ -std=c++17 -m32 -shared %COM_PARM% -o %DLL_NAME% %INP_NAME%
if %errorlevel% == 0 (echo Successfully Compiled!) else (goto error)
if %TES_MODE%==1 (
    echo Running...
    clang++ test.cpp -o test.exe
    if %errorlevel%==0 (test.exe) else (goto error)
)
goto done

:error
echo Compile error, see output above
goto done

:done