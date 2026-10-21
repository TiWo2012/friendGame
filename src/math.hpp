#pragma once

#include <cmath>

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
};

struct Color
{
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};

inline Vec2 operator+(Vec2 a, Vec2 b)
{
    return Vec2{a.x + b.x, a.y + b.y};
}

inline Vec2 operator-(Vec2 a, Vec2 b)
{
    return Vec2{a.x - b.x, a.y - b.y};
}

inline Vec2 operator*(Vec2 v, float scalar)
{
    return Vec2{v.x * scalar, v.y * scalar};
}

inline Vec2& operator+=(Vec2& a, Vec2 b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline float length(Vec2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline Vec2 normalize(Vec2 v)
{
    const float len = length(v);
    if (len == 0.0f)
    {
        return Vec2{};
    }
    return v * (1.0f / len);
}
