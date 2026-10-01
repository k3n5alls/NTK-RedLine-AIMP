$ErrorActionPreference = "Stop"

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw "CMake is not installed."
}

if (-not $env:CXX) { $env:CXX = "x86_64-w64-mingw32-g++" }
if (-not $env:CC)  { $env:CC  = "x86_64-w64-mingw32-gcc" }

cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j $env:NUMBER_OF_PROCESSORS

New-Item -ItemType Directory -Force package\x64 | Out-Null
Copy-Item build\aimp_NTKRedLine.dll package\x64\aimp_NTKRedLine.dll -Force

@"
NTK Red Line x64
AIMP 5.40 visualization
One red 1px waveform line, no background fill.
"@ | Set-Content package\ReadMe.txt -Encoding UTF8

Compress-Archive -Path package\* -DestinationPath NTK_RedLine_x64.zip -Force
Write-Host "BUILD OK"
