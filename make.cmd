@echo off
set a=
for /f usebackq %%i in (`dir /b /s *.c`) do call set a=%%a%% %%i
if "%a%" == "" (
    powershell write-host -fore red Error: No C source files found.
    exit /b 1
)
where gcc > nul 2>&1 || (
    powershell write-host -fore red Error: GCC compiler not found.
    exit /b 1
)
echo on

gcc -Wall -O3 -o kod.exe %a%

@echo off
echo.
if %errorlevel% neq 0 (
    powershell write-host -fore red Error: Compilation failed.
    exit /b 1
)
powershell write-host -fore green Compilation successful.