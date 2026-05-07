cmake_minimum_required(VERSION 4.0)
project(PengInt_GraphicsLibraries)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
set(OPENGL_VERSION "4.3" CACHE STRING "FORCE OPENGL 4.3" FORCE)
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(raylib)

add_executable(PengInt_GraphicsLibraries main.cpp
    include/PengInt_GUIL.hpp
    include/PengInt_UIL.hpp
)

target_link_libraries(PengInt_GraphicsLibraries
    raylib
    opengl32
    gdi32
    winmm
)