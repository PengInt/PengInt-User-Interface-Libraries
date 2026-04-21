cmake_minimum_required(VERSION 4.0)
project(PengInt_GraphicsLibraries)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(PengInt_GraphicsLibraries main.cpp
        include/PengInt_GUIL.hpp
        include/PengInt_UIL.hpp)

target_link_libraries(PengInt_GraphicsLibraries
        raylib
        opengl32
        gdi32
        winmm
)