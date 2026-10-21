#include "gfx.hpp"

#include "gl.hpp"

#include <cstdio>

namespace
{

constexpr const char* kVertexShaderSource = R"glsl(#version 330 core

layout(location = 0) in vec2 a_position;

uniform vec2 u_offset;
uniform float u_angle;
uniform float u_aspect;

void main()
{
    float s = sin(u_angle);
    float c = cos(u_angle);

    // Rotate in world space, then apply the camera's aspect correction.
    vec2 p = vec2(c * a_position.x - s * a_position.y,
                  s * a_position.x + c * a_position.y);
    p += u_offset;
    p.x /= u_aspect;

    gl_Position = vec4(p, 0.0, 1.0);
}
)glsl";

constexpr const char* kFragmentShaderSource = R"glsl(#version 330 core

uniform vec3 u_color;

out vec4 o_color;

void main()
{
    o_color = vec4(u_color, 1.0);
}
)glsl";

gl::Uint compile_shader(gl::Enum type, const char* source)
{
    const gl::Uint shader = gl::api.CreateShader(type);
    if (shader == 0)
    {
        std::fprintf(stderr, "gfx: could not create shader\n");
        return 0;
    }

    gl::api.ShaderSource(shader, 1, &source, nullptr);
    gl::api.CompileShader(shader);

    gl::Int compiled = 0;
    gl::api.GetShaderiv(shader, gl::COMPILE_STATUS, &compiled);
    if (compiled == 0)
    {
        char log[2048] = {};
        gl::api.GetShaderInfoLog(shader, static_cast<gl::Size>(sizeof(log)), nullptr, log);
        std::fprintf(stderr, "gfx: shader compilation failed:\n%s\n", log);
        gl::api.DeleteShader(shader);
        return 0;
    }

    return shader;
}

gl::Uint build_program()
{
    const gl::Uint vertex_shader = compile_shader(gl::VERTEX_SHADER, kVertexShaderSource);
    if (vertex_shader == 0)
    {
        return 0;
    }

    const gl::Uint fragment_shader = compile_shader(gl::FRAGMENT_SHADER, kFragmentShaderSource);
    if (fragment_shader == 0)
    {
        gl::api.DeleteShader(vertex_shader);
        return 0;
    }

    const gl::Uint program = gl::api.CreateProgram();
    if (program == 0)
    {
        gl::api.DeleteShader(vertex_shader);
        gl::api.DeleteShader(fragment_shader);
        std::fprintf(stderr, "gfx: could not create program\n");
        return 0;
    }

    gl::api.AttachShader(program, vertex_shader);
    gl::api.AttachShader(program, fragment_shader);
    gl::api.LinkProgram(program);

    gl::api.DeleteShader(vertex_shader);
    gl::api.DeleteShader(fragment_shader);

    gl::Int linked = 0;
    gl::api.GetProgramiv(program, gl::LINK_STATUS, &linked);
    if (linked == 0)
    {
        char log[2048] = {};
        gl::api.GetProgramInfoLog(program, static_cast<gl::Size>(sizeof(log)), nullptr, log);
        std::fprintf(stderr, "gfx: program link failed:\n%s\n", log);
        gl::api.DeleteProgram(program);
        return 0;
    }

    return program;
}

} // namespace

bool Gfx::init()
{
    program_ = build_program();
    if (program_ == 0)
    {
        return false;
    }

    u_offset_ = gl::api.GetUniformLocation(program_, "u_offset");
    u_angle_ = gl::api.GetUniformLocation(program_, "u_angle");
    u_aspect_ = gl::api.GetUniformLocation(program_, "u_aspect");
    u_color_ = gl::api.GetUniformLocation(program_, "u_color");

    if (u_offset_ < 0 || u_angle_ < 0 || u_aspect_ < 0 || u_color_ < 0)
    {
        std::fprintf(stderr, "gfx: shader uniforms were optimized out or renamed\n");
        shutdown();
        return false;
    }

    gl::api.GenVertexArrays(1, &vao_);
    gl::api.BindVertexArray(vao_);

    gl::api.GenBuffers(1, &vbo_);
    gl::api.BindBuffer(gl::ARRAY_BUFFER, vbo_);

    // Two floats per vertex, three vertices, in world space.
    constexpr float vertices[] = {
        -0.14f, -0.14f, 0.14f, -0.14f, 0.00f, 0.16f,
    };
    gl::api.BufferData(gl::ARRAY_BUFFER, static_cast<gl::SizeIptr>(sizeof(vertices)), vertices,
                       gl::STATIC_DRAW);

    gl::api.VertexAttribPointer(0, 2, gl::FLOAT, gl::FALSE,
                                static_cast<gl::Size>(2 * sizeof(float)), nullptr);
    gl::api.EnableVertexAttribArray(0);

    gl::api.BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::api.BindVertexArray(0);

    const auto* version = gl::api.GetString(gl::VERSION);
    const auto* renderer = gl::api.GetString(gl::RENDERER);
    std::printf("OpenGL %s | %s\n",
                version != nullptr ? reinterpret_cast<const char*>(version) : "unknown",
                renderer != nullptr ? reinterpret_cast<const char*>(renderer) : "unknown");

    return true;
}

void Gfx::shutdown()
{
    if (program_ != 0)
    {
        gl::api.DeleteProgram(program_);
        program_ = 0;
    }
    if (vbo_ != 0)
    {
        gl::api.DeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0)
    {
        gl::api.DeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    u_offset_ = -1;
    u_angle_ = -1;
    u_aspect_ = -1;
    u_color_ = -1;
}

void Gfx::begin_frame(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    aspect_ = static_cast<float>(width) / static_cast<float>(height);

    gl::api.Viewport(0, 0, width, height);
    gl::api.ClearColor(0.07f, 0.08f, 0.10f, 1.0f);
    gl::api.Clear(gl::COLOR_BUFFER_BIT);
}

void Gfx::draw_triangle(Vec2 center, float angle, Color color)
{
    gl::api.UseProgram(program_);
    gl::api.Uniform2f(u_offset_, center.x, center.y);
    gl::api.Uniform1f(u_angle_, angle);
    gl::api.Uniform1f(u_aspect_, aspect_);
    gl::api.Uniform3f(u_color_, color.r, color.g, color.b);

    gl::api.BindVertexArray(vao_);
    gl::api.DrawArrays(gl::TRIANGLES, 0, 3);
    gl::api.BindVertexArray(0);
}
