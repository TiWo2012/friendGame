cmake_minimum_required(VERSION 3.24)

project(friendGame VERSION 0.1.0 LANGUAGES C CXX)

# C++23 is the newest standard supported by current Apple Clang, Linux Clang, and MSVC.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# ---------------------------------------------------------------------------
# GLFW: windowing + input. CMake downloads and builds it from source.
# We only use the X11 backend on Linux to keep system requirements minimal
# (Wayland would additionally need wayland-scanner and libdecor).
# ---------------------------------------------------------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if(UNIX AND NOT APPLE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
    URL_HASH SHA256=b5ec004b2712fd08e8861dc271428f048775200a2df719ccf575143ba749a3e9
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(glfw)

# ---------------------------------------------------------------------------
# Game executable
# ---------------------------------------------------------------------------
add_executable(friend_game
    src/main.cpp
    src/game.cpp
    src/gfx.cpp
    src/gl.cpp
)

target_include_directories(friend_game PRIVATE src)
target_link_libraries(friend_game PRIVATE glfw)
target_compile_definitions(friend_game PRIVATE GLFW_INCLUDE_NONE)

# GL is only used through runtime-loaded function pointers, but linking the
# framework keeps direct GL calls working if they are added later.
if(APPLE)
    target_link_libraries(friend_game PRIVATE "-framework OpenGL")
endif()

option(FRIEND_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

function(friend_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /Zc:__cplusplus /utf-8)
        if(FRIEND_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
        if(FRIEND_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

friend_set_warnings(friend_game)
