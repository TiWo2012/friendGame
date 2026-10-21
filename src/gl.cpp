#include "gl.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>

namespace gl
{

Api api;

bool load()
{
    // Loaded through GLFW so the same path works on Windows, macOS, and Linux.
    // GLFW falls back to the system GL library for OpenGL 1.1 functions.
#define FRIEND_LOAD_GL(name)                                                                       \
    do                                                                                             \
    {                                                                                              \
        api.name = reinterpret_cast<decltype(api.name)>(glfwGetProcAddress("gl" #name));           \
        if (api.name == nullptr)                                                                   \
        {                                                                                          \
            std::fprintf(stderr, "gl: failed to load gl%s\n", #name);                              \
            return false;                                                                          \
        }                                                                                          \
    } while (false)

    FRIEND_LOAD_GL(ClearColor);
    FRIEND_LOAD_GL(Clear);
    FRIEND_LOAD_GL(Viewport);
    FRIEND_LOAD_GL(DrawArrays);

    FRIEND_LOAD_GL(GenVertexArrays);
    FRIEND_LOAD_GL(BindVertexArray);
    FRIEND_LOAD_GL(DeleteVertexArrays);
    FRIEND_LOAD_GL(GenBuffers);
    FRIEND_LOAD_GL(BindBuffer);
    FRIEND_LOAD_GL(BufferData);
    FRIEND_LOAD_GL(DeleteBuffers);
    FRIEND_LOAD_GL(VertexAttribPointer);
    FRIEND_LOAD_GL(EnableVertexAttribArray);

    FRIEND_LOAD_GL(CreateShader);
    FRIEND_LOAD_GL(ShaderSource);
    FRIEND_LOAD_GL(CompileShader);
    FRIEND_LOAD_GL(GetShaderiv);
    FRIEND_LOAD_GL(GetShaderInfoLog);
    FRIEND_LOAD_GL(DeleteShader);

    FRIEND_LOAD_GL(CreateProgram);
    FRIEND_LOAD_GL(AttachShader);
    FRIEND_LOAD_GL(LinkProgram);
    FRIEND_LOAD_GL(GetProgramiv);
    FRIEND_LOAD_GL(GetProgramInfoLog);
    FRIEND_LOAD_GL(DeleteProgram);
    FRIEND_LOAD_GL(UseProgram);
    FRIEND_LOAD_GL(GetUniformLocation);
    FRIEND_LOAD_GL(Uniform1f);
    FRIEND_LOAD_GL(Uniform2f);
    FRIEND_LOAD_GL(Uniform3f);

    FRIEND_LOAD_GL(GetString);

#undef FRIEND_LOAD_GL

    return true;
}

} // namespace gl
