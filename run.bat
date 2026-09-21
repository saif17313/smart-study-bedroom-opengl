@echo off
setlocal
if not exist "%~dp0build\smart_study_bedroom.exe" (
    echo Build the project first using build.bat.
    pause
    exit /b 1
)
pushd "%~dp0build"
smart_study_bedroom.exe
set "bedroomExit=%ERRORLEVEL%"
popd
if not "%bedroomExit%"=="0" pause
exit /b %bedroomExit%
