cmake_minimum_required(VERSION 4.0)
cmake_policy(SET CMP0169 OLD)
project(UserProject)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

include_directories(${CMAKE_SOURCE_DIR}/../../include)
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

add_executable(${PROJECT_NAME} runner.cpp)

if (NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
    target_include_directories(${PROJECT_NAME} PRIVATE "${rapidjson_SOURCE_DIR}/include")
endif()

set_target_properties(UserProject PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
)

if (WIN32)
    target_link_libraries(UserProject
        raylib
        opengl32
        gdi32
        winmm
    )
else()
    target_link_libraries(UserProject
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