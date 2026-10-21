#pragma once

#include <cstddef>

// OpenGL functions use the stdcall convention on 32-bit Windows.
#if defined(_WIN32)
#define FRIEND_GLAPI __stdcall
#else
#define FRIEND_GLAPI
#endif

namespace gl
{

using Enum = unsigned int;
using Bitfield = unsigned int;
using Int = int;
using Size = int;
using Uint = unsigned int;
using Boolean = unsigned char;
using Float = float;
using Char = char;
using Ubyte = unsigned char;
using SizeIptr = std::ptrdiff_t;

// Only the constants the game uses. Keeping this file small avoids pulling in
// any platform OpenGL headers.
inline constexpr Enum TRIANGLES = 0x0004u;
inline constexpr Bitfield COLOR_BUFFER_BIT = 0x00004000u;
inline constexpr Enum ARRAY_BUFFER = 0x8892u;
inline constexpr Enum STATIC_DRAW = 0x88E4u;
inline constexpr Enum FLOAT = 0x1406u;
inline constexpr Boolean FALSE = 0u;
inline constexpr Enum VERTEX_SHADER = 0x8B31u;
inline constexpr Enum FRAGMENT_SHADER = 0x8B30u;
inline constexpr Enum COMPILE_STATUS = 0x8B81u;
inline constexpr Enum LINK_STATUS = 0x8B82u;
inline constexpr Enum VERSION = 0x1F02u;
inline constexpr Enum RENDERER = 0x1F01u;

// The subset of the OpenGL 3.3 core API used by this project. Every pointer is
// resolved at runtime through glfwGetProcAddress, so no GL library is linked.
struct Api
{
    void(FRIEND_GLAPI* ClearColor)(Float red, Float green, Float blue, Float alpha) = nullptr;
    void(FRIEND_GLAPI* Clear)(Bitfield mask) = nullptr;
    void(FRIEND_GLAPI* Viewport)(Int x, Int y, Size width, Size height) = nullptr;
    void(FRIEND_GLAPI* DrawArrays)(Enum mode, Int first, Size count) = nullptr;

    void(FRIEND_GLAPI* GenVertexArrays)(Size count, Uint* arrays) = nullptr;
    void(FRIEND_GLAPI* BindVertexArray)(Uint array) = nullptr;
    void(FRIEND_GLAPI* DeleteVertexArrays)(Size count, const Uint* arrays) = nullptr;
    void(FRIEND_GLAPI* GenBuffers)(Size count, Uint* buffers) = nullptr;
    void(FRIEND_GLAPI* BindBuffer)(Enum target, Uint buffer) = nullptr;
    void(FRIEND_GLAPI* BufferData)(Enum target, SizeIptr size, const void* data,
                                   Enum usage) = nullptr;
    void(FRIEND_GLAPI* DeleteBuffers)(Size count, const Uint* buffers) = nullptr;
    void(FRIEND_GLAPI* VertexAttribPointer)(Uint index, Int size, Enum type, Boolean normalized,
                                            Size stride, const void* pointer) = nullptr;
    void(FRIEND_GLAPI* EnableVertexAttribArray)(Uint index) = nullptr;

    Uint(FRIEND_GLAPI* CreateShader)(Enum type) = nullptr;
    void(FRIEND_GLAPI* ShaderSource)(Uint shader, Size count, const Char* const* source,
                                     const Int* length) = nullptr;
    void(FRIEND_GLAPI* CompileShader)(Uint shader) = nullptr;
    void(FRIEND_GLAPI* GetShaderiv)(Uint shader, Enum name, Int* value) = nullptr;
    void(FRIEND_GLAPI* GetShaderInfoLog)(Uint shader, Size bufSize, Size* length,
                                         Char* infoLog) = nullptr;
    void(FRIEND_GLAPI* DeleteShader)(Uint shader) = nullptr;

    Uint(FRIEND_GLAPI* CreateProgram)(void) = nullptr;
    void(FRIEND_GLAPI* AttachShader)(Uint program, Uint shader) = nullptr;
    void(FRIEND_GLAPI* LinkProgram)(Uint program) = nullptr;
    void(FRIEND_GLAPI* GetProgramiv)(Uint program, Enum name, Int* value) = nullptr;
    void(FRIEND_GLAPI* GetProgramInfoLog)(Uint program, Size bufSize, Size* length,
                                          Char* infoLog) = nullptr;
    void(FRIEND_GLAPI* DeleteProgram)(Uint program) = nullptr;
    void(FRIEND_GLAPI* UseProgram)(Uint program) = nullptr;
    Int(FRIEND_GLAPI* GetUniformLocation)(Uint program, const Char* name) = nullptr;
    void(FRIEND_GLAPI* Uniform1f)(Int location, Float value) = nullptr;
    void(FRIEND_GLAPI* Uniform2f)(Int location, Float x, Float y) = nullptr;
    void(FRIEND_GLAPI* Uniform3f)(Int location, Float x, Float y, Float z) = nullptr;

    const Ubyte*(FRIEND_GLAPI* GetString)(Enum name) = nullptr;
};

extern Api api;

// Requires a current OpenGL context.
bool load();

} // namespace gl

#undef FRIEND_GLAPI
