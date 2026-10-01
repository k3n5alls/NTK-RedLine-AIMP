cmake_minimum_required(VERSION 3.20)
project(NTK_RedLine LANGUAGES CXX)

if(NOT WIN32)
  message(FATAL_ERROR "This plugin must be built on Windows with MinGW-w64 or MSVC.")
endif()

add_library(aimp_NTKRedLine SHARED
  src/NTKRedLine.cpp
)

target_include_directories(aimp_NTKRedLine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/cpp
)

target_compile_features(aimp_NTKRedLine PRIVATE cxx_std_17)
target_compile_definitions(aimp_NTKRedLine PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)

if(MINGW)
  target_link_options(aimp_NTKRedLine PRIVATE
    -static
    -static-libgcc
    -static-libstdc++
  )
endif()

set_target_properties(aimp_NTKRedLine PROPERTIES
  OUTPUT_NAME "aimp_NTKRedLine"
  PREFIX ""
)

install(TARGETS aimp_NTKRedLine
  RUNTIME DESTINATION x64
)
