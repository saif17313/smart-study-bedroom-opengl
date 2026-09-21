@echo off
setlocal
rem Use the existing MSYS2 UCRT64 toolchain when available.
if exist "C:\msys64\ucrt64\bin\cmake.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
cmake -S "%~dp0." -B "%~dp0build" -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto failed
cmake --build "%~dp0build" --parallel 4
if errorlevel 1 goto failed
echo Build complete. Open run.bat to start the bedroom.
exit /b 0
:failed
echo Build failed. Check that GCC, CMake and Ninja are installed.
pause
exit /b 1
