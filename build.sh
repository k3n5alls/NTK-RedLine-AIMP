#!/usr/bin/env bash
set -e
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc 2>/dev/null || echo 4)"
mkdir -p package/x64
cp build/aimp_NTKRedLine.dll package/x64/
printf 'NTK Red Line x64\nAIMP 5.40 visualization\nOne red 1px waveform line, no background fill.\n' > package/ReadMe.txt
cd package
zip -r ../NTK_RedLine_x64.zip .
