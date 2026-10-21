#pragma once

#include "math.hpp"

class Gfx
{
public:
    Gfx() = default;
    Gfx(const Gfx&) = delete;
    Gfx& operator=(const Gfx&) = delete;

    bool init();
    void shutdown();

    void begin_frame(int width, int height);
    void draw_triangle(Vec2 center, float angle, Color color);

private:
    unsigned int program_ = 0;
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;

    int u_offset_ = -1;
    int u_angle_ = -1;
    int u_aspect_ = -1;
    int u_color_ = -1;

    float aspect_ = 1.0f;
};
