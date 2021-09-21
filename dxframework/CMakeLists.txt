# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("dxframework")

# Include sub-projects.
set(BUILD_XAUDIO_WIN10 ON CACHE BOOL "Build for XAudio 2.9")
set(BUILD_XAUDIO_WIN8 OFF CACHE BOOL "Build for XAudio 2.8")
add_subdirectory ("thirdparty/DirectXTK")

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build your own zlib")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build package with shared libraries.")
add_subdirectory ("thirdparty/assimp")

add_subdirectory ("dxframework")
