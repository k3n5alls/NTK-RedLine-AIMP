# NTK Red Line x64 — free-build edition

Target: AIMP 5.40 x64.

This is a clean-room C++17 implementation using the official AIMP 5.40 C++ SDK headers.

## What it draws

Exactly one thing: a 1-pixel red waveform line.

- Color: #E62623
- No background fill
- No glow
- No grid
- No spectrum
- Left/right waveform averaged into one line
- Native x64 DLL

## Build for free

### Option A — MSYS2 + MinGW-w64 (recommended)
Install MSYS2, then in the UCRT64 shell:

    pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake
    ./build.sh

For a normal Windows command prompt you can use the included build.bat after
the MinGW-w64 bin directory is on PATH.

### Option B — Visual Studio Community
Visual Studio Community is free for individual developers. Open the CMake project
and build x64/Release.

### Option C — GitHub Actions
This repository includes `.github/workflows/build.yml`. Push the folder to a
public GitHub repository and run the workflow. The Windows runner compiles the
x64 DLL and uploads the DLL + ZIP as an artifact.

## Install

AIMP can install plugins from an AIMP addon package. The package produced here is:

    NTK_RedLine_x64.zip

Its contents are:

    x64/aimp_NTKRedLine.dll
    ReadMe.txt

If AIMP rejects a ZIP as an addon package in your build, copy the DLL manually to
the AIMP Plugins folder. Current third-party AIMP projects document both package
and direct DLL installation.

## For another AI/developer

The important files are:

- src/NTKRedLine.cpp — all plugin and rendering code
- sdk/cpp/apiVisuals.h — exact AIMP 5.40 visual API
- CMakeLists.txt — build definition
- build.bat / build.ps1 — local build
- .github/workflows/build.yml — cloud build

The rendering code is intentionally tiny so it is easy to modify:
color, line width, smoothing, amplitude and channel mixing are all in one file.
