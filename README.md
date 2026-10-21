# friendGame

A small, low-level C++ game skeleton that builds on Linux, macOS, and Windows.

- **C++23**, the newest standard supported by current Linux Clang, Apple Clang, and MSVC
- **One third-party dependency:** [GLFW 3.4](https://www.glfw.org/), downloaded and built from source by CMake
- **Raw OpenGL 3.3** with a tiny, hand-written function loader (no glad, no engine)
- **C with classes** style: simple structs, a couple of classes, no STL containers, no exceptions or RTTI required
- CMake presets for native Linux/macOS/MSVC builds and a MinGW cross-compile preset

## Requirements

Build tools:

- CMake 3.24 or newer
- Ninja for the Linux presets (macOS uses the system Makefiles generator)
- A C++23-capable compiler:
  - Linux: Clang 17+
  - macOS: Apple Clang 16+ (Xcode 16+)
  - Windows: MSVC 19.35+ (Visual Studio 2022 17.5+)

Platform SDK pieces (these are OS libraries, not project dependencies):

- **Linux:** X11 development headers. On Debian/Ubuntu:
  ```sh
  sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev
  ```
  On Arch:
  ```sh
  sudo pacman -S libx11 libxrandr libxinerama libxcursor libxi libxext
  ```
  GLFW is configured X11-only to avoid the extra Wayland tooling; on Wayland desktops it runs through XWayland.
- **macOS:** Xcode command line tools (`xcode-select --install`).
- **Windows:** Visual Studio with the "Desktop development with C++" workload. No vcpkg needed.

GLFW is the only fetched dependency. Everything else is the platform SDK and the C/C++ runtime.
The first configure needs network access to download GLFW 3.4; it is pinned by SHA-256.
Add `-DFRIEND_WARNINGS_AS_ERRORS=ON` to a configure command to treat warnings as errors.

## Build

### Linux (Clang)

```sh
cmake --preset linux-clang
cmake --build build/linux-clang
./build/linux-clang/friend_game
```

For an optimized build:

```sh
cmake --preset linux-clang-release
cmake --build build/linux-clang-release
```

### macOS (Apple Clang)

```sh
cmake --preset macos-clang
cmake --build build/macos-clang
./build/macos-clang/friend_game
```

### Windows (MSVC)

Run from a normal terminal or a Developer Command Prompt:

```powershell
cmake --preset windows-msvc
cmake --build build/windows-msvc --config Release
.\build\windows-msvc\Release\friend_game.exe
```

The Windows preset uses CMake's default Visual Studio generator, so it adapts to the installed Visual Studio version.

## Controls

| Input | Action |
| --- | --- |
| W / A / S / D | Move the triangle |
| Esc | Quit |

## Cross compilation

A MinGW-w64 toolchain file is included. Install the cross compiler (`sudo apt install mingw-w64`), then:

```sh
cmake --preset windows-mingw-cross
cmake --build build/windows-mingw-cross
```

The result is `build/windows-mingw-cross/friend_game.exe`. Windows DLLs from the MinGW runtime may be needed when running it on another machine.

For other targets (aarch64 Linux, osxcross, etc.), pass a toolchain file directly:

```sh
cmake -S . -B build/my-target \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/my-target
```

There is no host-specific code in the game target: GLFW selects the correct windowing backend for each toolchain.

## Project layout

```
CMakeLists.txt              Build definition; fetches and builds GLFW
CMakePresets.json           Native and cross-compile presets
cmake/toolchains/           MinGW-w64 toolchain file
src/main.cpp                Entry point
src/game.hpp/.cpp           Window, game loop, input
src/gfx.hpp/.cpp            Shader program and triangle rendering
src/gl.hpp/.cpp             Minimal OpenGL 3.3 loader
src/math.hpp                Vec2, Color, small helpers
.github/workflows/build.yml Linux, macOS, and Windows CI builds
```

## Design notes

- **Why only one dependency?** GLFW is small, C, and limited to windowing, input, and contexts. OpenGL itself comes from the driver. There is no asset loader, audio library, or entity framework to work around.
- **Why a custom GL loader?** The game uses about 30 GL functions. Loading them through `glfwGetProcAddress` avoids a generated loader dependency and keeps the rendering path explicit.
- **Why C++23?** It is the newest standard all three target compilers can compile. The code intentionally avoids newer/convenient-but-heavy features; it reads like C with classes.
- **Rendering path:** one VAO/VBO, one shader program, a spinning triangle. It is a starting point, not an engine.
- **Window backend:** GLFW is built statically. At runtime the executable uses the OS windowing libraries (X11, Win32, or Cocoa).

## CI

`.github/workflows/build.yml` builds the project with Clang on Linux, Apple Clang on macOS, and MSVC on Windows.
