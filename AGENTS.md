# AGENTS.md

Instructions for AI coding agents working on `friendGame`. These rules are mandatory. If a user request conflicts with them, point out the conflict and get explicit confirmation before proceeding.

## Read first

Before changing anything, read `README.md`, `CMakeLists.txt`, `CMakePresets.json`, and the code around the change. Match existing patterns instead of inventing new ones.

## Project snapshot

- C++23, but deliberately written in a simple "C with classes" subset.
- One third-party dependency: GLFW 3.4, fetched and built by CMake.
- Raw OpenGL 3.3 core with a hand-written loader in `src/gl.hpp` / `src/gl.cpp`.
- Targets: Linux Clang, Apple Clang, MSVC, plus a MinGW-w64 cross-compile preset.
- No unit-test framework yet. Verification is build + smoke run + CI.

## Build and verification

Local Linux workflow:

```sh
cmake --preset linux-clang -DFRIEND_WARNINGS_AS_ERRORS=ON
cmake --build build/linux-clang
clang-format --dry-run --Werror src/*.cpp src/*.hpp
./build/linux-clang/friend_game   # when a display is available
```

Rules:

- C++ or CMake changes MUST compile with warnings as errors (`FRIEND_WARNINGS_AS_ERRORS=ON`) before being committed.
- Graphics, input, or game-loop changes MUST be smoke-tested by running the game when a display is available. If it cannot be run, say so explicitly.
- Build the Release preset too when changing CMake, dependencies, build flags, or optimization-sensitive code.
- Do not claim a platform was tested when it was not. MSVC and Apple Clang are covered by CI; local Linux testing does not prove them.
- Keep build artifacts inside `build/<preset>/`. Never commit them.
- Do not add a test framework without explicit user approval. If tests are added later, wire them into CTest and CI.
- Never disable a CI job or compiler warning just to make a change pass. Fix the cause.

## Code style

`clang-format` is the source of truth for formatting. The project uses 4-space indentation, Allman braces, LF line endings, a 100-column limit, and no trailing whitespace. Run clang-format on every changed C++ file; never hand-format against it.

### Language subset

The standard is C++23, but the code should remain readable as "C with classes". Do not introduce:

- exceptions (`throw`, `try`, `catch`) or exception-based error handling
- RTTI, `typeid`, `dynamic_cast`, inheritance, or virtual functions
- smart pointers, `std::function`, or ownership wrappers
- STL containers or algorithms (`std::vector`, `std::array`, `std::string`, `std::span`, etc.)
- iostreams
- lambdas in game code
- complex templates or metaprogramming

Prefer plain structs, C arrays, free functions, and small classes with explicit `init` / `update` / `shutdown` methods. Simple `constexpr`, `[[nodiscard]]`, scoped enums, and operator overloads on small math types are welcome. If a feature from the "do not introduce" list seems necessary, ask first.

Other rules:

- Heap allocation is not allowed in per-frame or rendering code. Prefer fixed-capacity arrays and value types.
- Use `float` for gameplay math. Convert GLFW's `double` timestamps at the boundary.
- Use `nullptr`, `const`, `constexpr`, and explicit C++ casts (`static_cast`, `reinterpret_cast`). No C-style casts, `NULL`, or `0` as a pointer.
- Do not use `using namespace`.
- Avoid `auto` unless the type is obvious from the right-hand side or is painful to spell. Do not use `auto` to hide ownership.
- Keep functions short and use early returns. Do not deeply nest conditionals.
- Do not add global mutable state other than the OpenGL API table in `gl::api`.
- Comments explain **why**, not **what**. Do not commit commented-out code, changelog comments, or unexplained `TODO` / `FIXME` markers.

### Naming

| Item | Convention | Examples |
| --- | --- | --- |
| Types, classes, structs | `PascalCase` | `Game`, `Gfx`, `Vec2`, `Color` |
| Functions and methods | `snake_case` | `draw_triangle`, `begin_frame`, `init` |
| Local variables and parameters | `snake_case` | `delta_time`, `window_width` |
| Member variables | `snake_case_` | `window_`, `program_`, `player_position_` |
| Constants | `kPascalCase` | `kWindowWidth`, `kMaxDeltaTime` |
| Macros | `FRIEND_UPPER_SNAKE` | `FRIEND_GLAPI`, `FRIEND_LOAD_GL` |
| Files | `lower_snake_case.hpp/.cpp` | `game.cpp`, `gfx.hpp` |
| Namespaces | lowercase | `gl` |

Exception: the OpenGL loader mirrors GL naming. Its API fields are `PascalCase` (`gl::api.ClearColor`), its constants are `gl::UPPER_SNAKE` (`gl::TRIANGLES`), and its type aliases mirror GL types (`gl::Uint`). Keep that pattern when extending the loader.

### Files and headers

- One primary class per header/source pair; the file name lowercases the class name (`Game` -> `game.hpp`, `Gfx` -> `gfx.hpp`).
- Use `#pragma once`, never include guards.
- Put file-local helpers in an anonymous namespace in the `.cpp`.
- Keep headers minimal. Forward-declare when possible (as `game.hpp` does with `GLFWwindow`).
- Include order: own header, other project headers, third-party headers, then standard library. Separate groups with a blank line.
- Do not include OpenGL, X11, Windows, or Cocoa headers in game code. Use the `gl` loader and GLFW APIs.
- Add every new `.cpp` explicitly to `add_executable` in `CMakeLists.txt`. Never use file globs.

### Errors and logging

- Fallible initialization returns `bool`. Top-level initialization is marked `[[nodiscard]]`.
- Log to `stderr` with `std::fprintf` and a lowercase subsystem prefix: `game:`, `gfx:`, or `gl:`.
- Clean up partially initialized resources on failure.
- `main` returns `1` when initialization fails.
- Do not call `exit()`, `abort()`, or throw from game code. Do not silently ignore a failed GLFW or OpenGL call that affects correctness.

### OpenGL rules

- Target OpenGL 3.3 core and GLSL `#version 330 core`.
- Every GL function must go through `gl::api`. To add one, declare its pointer in `gl.hpp`, load it in `gl.cpp`, then use it.
- Do not use immediate mode, fixed-function pipeline calls, compatibility-profile features, or direct GL symbol linking.
- Keep shader sources embedded in `gfx.cpp` unless an asset pipeline is explicitly requested.
- Never compile shaders or recreate buffers per frame.
- Every GL resource must be released in the matching `shutdown` path, before the window/context is destroyed. Guard deletion with non-zero handle checks.
- Keep the loader subset small. Do not paste in a generated glad loader.

### Resource lifetime

`Game` owns the window and `Gfx`. Order is fixed:

1. Initialize GLFW and create the window.
2. Make the context current and load GL.
3. Initialize `Gfx`.
4. Destroy GL resources.
5. Destroy the window.
6. Terminate GLFW.

Do not call OpenGL from a destructor unless the context is guaranteed to still be current. Prefer explicit `shutdown` called by `Game`.

## CMake and dependencies

- GLFW is the only permitted third-party library. Do not replace it, vendor alternatives, or add another dependency without explicit user approval.
- If a dependency is approved, fetch and build it from source with `FetchContent`, pin an exact version and SHA-256, and disable its docs, tests, examples, and install rules where possible.
- The build must not require vcpkg, Conan, or project-specific system packages. Only the OS SDK, C/C++ runtime, and normal platform windowing headers are allowed.
- Never require the user to install a new package to make the build pass. Report the missing prerequisite instead.
- Linux stays X11-only for now. Do not enable Wayland or add Wayland tooling unless explicitly asked.
- Keep `CMAKE_CXX_STANDARD` at 23 with extensions off.
- Preserve existing target and preset names. Add new presets instead of renaming existing ones.
- Keep the warning flags and the `FRIEND_WARNINGS_AS_ERRORS` option working.
- Update `README.md`, `CMakePresets.json`, and `.github/workflows/build.yml` whenever build commands, prerequisites, targets, or platform behavior change.

## Git conventions

Repository default: `main`, with `origin` at `git@github.com:TiWo2012/friendGame.git`.

### Commits

Use Conventional Commits:

```text
<type>(<scope>): <imperative summary>
```

- Types: `feat`, `fix`, `perf`, `refactor`, `docs`, `build`, `ci`, `test`, `style`, `chore`.
- Scopes are optional but preferred: `game`, `gfx`, `gl`, `math`, `cmake`, `ci`, `docs`.
- Subject: imperative mood, lowercase after the colon, at most 72 characters, no trailing period.
- Add a blank line and a body when the reason is not obvious. Wrap the body at 72 characters and explain why and what was verified.
- One logical change per commit. Do not mix formatting with behavior changes.
- No agent attribution, emoji, or "generated by" footers.
- Breaking changes use `!` in the type/scope and a `BREAKING CHANGE:` footer.

Examples:

```text
feat(game): add player wrapping at screen edges
fix(gfx): handle zero-sized framebuffers
build(cmake): pin the GLFW archive hash
docs: document the MinGW cross build
style: apply clang-format to src
```

### Workflow

- Commit only completed, verified work. No work-in-progress commits unless the user asks for one.
- Never end a response with uncommitted task changes. Before finishing any response that modified or added files, commit the completed work. `git status --short` must show no task-related modifications, staged changes, or untracked files; ignored build output is fine.
- Small single-commit changes may go directly on `main`. Use a short-lived topic branch for multi-commit or risky work: `<type>/<kebab-case-summary>`, e.g. `feat/double-jump`.
- Keep history linear. Rebase topic branches onto `main`; do not create merge commits.
- Never rewrite or force-push `main`. On an unpushed branch of your own, `--force-with-lease` is acceptable.
- Never amend a commit that has already been pushed.
- Do not bypass hooks or checks with `--no-verify` or equivalent.
- Stage explicit paths. Before committing, review `git status`, `git diff`, and `git diff --cached`. Never stage `build/`, `_deps/`, caches, `compile_commands.json`, `CMakeUserPresets.json`, binaries, or secrets.
- Leave pre-existing user changes alone. Never revert or reformat files unrelated to the task.
- Do not commit large binary assets without explicit permission.
- Do not push, change remotes, rewrite tags, or modify Git configuration unless the user explicitly asks. Pushing is a separate, deliberate action.

## Definition of done

Before considering a change complete:

- [ ] The code matches the style and naming rules above.
- [ ] `clang-format --dry-run --Werror` reports no issues.
- [ ] The affected preset builds with warnings as errors.
- [ ] Graphics/input changes were smoke-tested, or the lack of a display was reported.
- [ ] README, presets, or CI were updated if behavior or commands changed.
- [ ] `git diff` contains only task-related changes and no generated files.
- [ ] All task changes are committed before the response ends; `git status --short` shows no unexpected files.
- [ ] The commit follows the commit rules above.
