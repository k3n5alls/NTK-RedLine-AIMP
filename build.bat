@echo off
setlocal
where cmake >nul 2>nul
if errorlevel 1 (
  echo CMake not found.
  echo Install CMake and MSYS2 MinGW-w64, then run this again.
  exit /b 1
)

if "%CXX%"=="" set "CXX=x86_64-w64-mingw32-g++"
if "%CC%"=="" set "CC=x86_64-w64-mingw32-gcc"

if not exist build mkdir build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1

cmake --build build --config Release -j
if errorlevel 1 exit /b 1

if not exist package\x64 mkdir package\x64
copy /Y build\aimp_NTKRedLine.dll package\x64\aimp_NTKRedLine.dll >nul

(
  echo NTK Red Line x64
  echo AIMP 5.40 visualization
  echo One red 1px waveform line, no background fill.
) > package\ReadMe.txt

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "Compress-Archive -Path 'package\*' -DestinationPath 'NTK_RedLine_x64.zip' -Force"

echo.
echo BUILD OK
echo Output: build\aimp_NTKRedLine.dll
echo Package: NTK_RedLine_x64.zip
