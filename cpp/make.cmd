@echo off
set a=
for /f usebackq %%i in (`dir /b /s *.cpp`) do call set a=%%a%% %%i
if "%a%" == "" (
    powershell write-host -fore red Error: No C source files found.
    exit /b 1
)
where g++ > nul 2>&1 || (
    powershell write-host -fore red Error: GCC compiler not found.
    exit /b 1
)
echo on

g++ -ggdb -Wall -o kod.exe %a% -I./include

@echo off
echo.
if %errorlevel% neq 0 (
    powershell write-host -fore red Error: Compilation failed.
    exit /b 1
)
powershell write-host -fore green Compilation successful.