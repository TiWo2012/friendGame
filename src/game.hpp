#pragma once

#include "gfx.hpp"
#include "math.hpp"

struct GLFWwindow;

class Game
{
public:
    Game() = default;
    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    [[nodiscard]] bool init();
    void run();

private:
    void update(float dt);
    void render();
    void shutdown();

    GLFWwindow* window_ = nullptr;
    Gfx gfx_;

    Vec2 player_position_{};
    float player_angle_ = 0.0f;
    bool glfw_ready_ = false;
};
