#include "game.hpp"

#include "gl.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>

namespace
{

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr float kMoveSpeed = 0.9f;
constexpr float kSpinSpeed = 1.2f;
constexpr float kMaxDeltaTime = 0.1f;
constexpr float kWrapX = 1.75f;
constexpr float kWrapY = 0.95f;

void glfw_error_callback(int error, const char* description)
{
    std::fprintf(stderr, "glfw error %d: %s\n", error, description);
}

} // namespace

Game::~Game()
{
    shutdown();
}

bool Game::init()
{
    if (glfw_ready_)
    {
        return true;
    }

    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() == GLFW_FALSE)
    {
        std::fprintf(stderr, "game: glfwInit failed\n");
        return false;
    }
    glfw_ready_ = true;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window_ = glfwCreateWindow(kWindowWidth, kWindowHeight, "Friend Game", nullptr, nullptr);
    if (window_ == nullptr)
    {
        std::fprintf(stderr, "game: could not create a window with an OpenGL 3.3 context\n");
        shutdown();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    if (!gl::load())
    {
        shutdown();
        return false;
    }

    if (!gfx_.init())
    {
        shutdown();
        return false;
    }

    return true;
}

void Game::run()
{
    if (window_ == nullptr)
    {
        return;
    }

    double last_time = glfwGetTime();

    while (glfwWindowShouldClose(window_) == GLFW_FALSE)
    {
        glfwPollEvents();

        const double now = glfwGetTime();
        float delta_time = static_cast<float>(now - last_time);
        last_time = now;

        if (delta_time < 0.0f)
        {
            delta_time = 0.0f;
        }
        if (delta_time > kMaxDeltaTime)
        {
            delta_time = kMaxDeltaTime;
        }

        update(delta_time);
        render();

        glfwSwapBuffers(window_);
    }
}

void Game::update(float dt)
{
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }

    Vec2 direction{};
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    {
        direction.x -= 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    {
        direction.x += 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    {
        direction.y += 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    {
        direction.y -= 1.0f;
    }

    if (direction.x != 0.0f || direction.y != 0.0f)
    {
        player_position_ += normalize(direction) * (kMoveSpeed * dt);
    }

    player_angle_ += kSpinSpeed * dt;

    if (player_position_.x > kWrapX)
    {
        player_position_.x = -kWrapX;
    }
    else if (player_position_.x < -kWrapX)
    {
        player_position_.x = kWrapX;
    }

    if (player_position_.y > kWrapY)
    {
        player_position_.y = -kWrapY;
    }
    else if (player_position_.y < -kWrapY)
    {
        player_position_.y = kWrapY;
    }
}

void Game::render()
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    if (width <= 0 || height <= 0)
    {
        return;
    }

    gfx_.begin_frame(width, height);
    gfx_.draw_triangle(player_position_, player_angle_, Color{0.95f, 0.72f, 0.25f});
}

void Game::shutdown()
{
    gfx_.shutdown();

    if (window_ != nullptr)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfw_ready_)
    {
        glfwTerminate();
        glfw_ready_ = false;
    }
}
