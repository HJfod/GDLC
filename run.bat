@echo off

set INP_NAME=dllmain.cpp
set DLL_NAME=GDLiveCollab.dll
set EXE_NAME=GDLiveCollab.exe
set COM_PARM=-luser32 -lDbghelp -llibMinHook-x86-v141-mt
set RUN_ARGS=""
set DLL_COMP=1
set RUN_TRUE=1
set PUB_LISH=0

FOR %%A IN (%*) DO (
    IF "%%A"=="-a" set RUN_ARGS=""
    IF "%%A"=="-r" set DLL_COMP=0
    IF "%%A"=="-c" set RUN_TRUE=0
    IF "%%A"=="-p" set PUB_LISH=1
)

if %DLL_COMP%==1 (
    echo Compiling DLL...
    clang++ -std=c++17 -m32 -shared %COM_PARM% -o %DLL_NAME% %INP_NAME%
    if %errorlevel% == 0 (echo Compiled DLL!) else (goto error)
)
echo Compiling Runner...
clang++ runner.cpp -o %EXE_NAME% -std=c++17 -m32 -luser32 -lWtsApi32
if %errorlevel%==0 (
    echo Compiled Runner!
    if %PUB_LISH%==1 (
        goto publish
    )
    if %RUN_TRUE%==1 (
        echo.
        %EXE_NAME%
    )
) else (goto error)
goto done

:error
echo Compile error, see output above
goto done

:publish
echo Publishing...
md releases
xcopy /y GDLiveCollab.exe releases\GDLiveCollab.exe*
xcopy /y GDLiveCollab.dll releases\GDLiveCollab.dll*
xcopy /y MinHook.x86.dll releases\MinHook.x86.dll*
goto done

:done