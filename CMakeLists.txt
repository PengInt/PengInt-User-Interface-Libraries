cmake_minimum_required(VERSION 4.0)
cmake_policy(SET CMP0169 OLD)
project(QED_GraphicsLibraries)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
include(FetchContent)
FetchContent_Declare(
    rapidjson
    URL https://github.com/Tencent/rapidjson/archive/refs/heads/master.zip
)

add_executable(QED_GraphicsLibraries main.cpp
        include/QED_R-GUIL.hpp
        include/QED_UIL.hpp
        include/QED_FGE.hpp
        include/FGE_SourceCodeHandler.hpp
)

if (NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
    target_include_directories(QED_GraphicsLibraries PRIVATE "${rapidjson_SOURCE_DIR}/include")
endif()

if (WIN32)
    target_link_libraries(QED_GraphicsLibraries
        raylib
        opengl32
        gdi32
        winmm
    )
else()
    target_link_libraries(QED_GraphicsLibraries
        raylib
        GL
        X11
        Xrandr
        Xinerama
        Xcursor
        Xi
        pthread
        dl
    )
endif()