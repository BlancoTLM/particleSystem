#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <array>
#include <numbers>
#include <glm/glm.hpp>

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    float mass;
    float age = 0.0f;
    float lifetime = 0.0f;
    glm::vec4 color_start;
    glm::vec4 color_end;

    Particle() {
    
        float Rmax = 1.0f;
        float u = utils::rand(0.0f, 1.0f);
        float v = utils::rand(0.0f, 1.0f);

        float r = Rmax * std::sqrt(u);
        float theta = 2.0f * std::numbers::pi * v;

        position = glm::vec2{
            r * std::cos(theta),
         r * std::sin(theta)
        };

        float angle = utils::rand(0.0f, 360.0f);
        float speed = utils::rand(0.1f, 0.2f);

        velocity = glm::vec2{
            std::cos(angle) * speed,
            std::sin(angle) * speed
        };

        color_start = glm::vec4
        {
            1.0f,
            1.0f,
            1.0f,
            1.0f
        };
    }
};

void draw_parametric(std::function<glm::vec2(float)> const& parametric)
{
    const float step = 0.01f;
    glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

    for (float t = 0.0f; t < 1.0f; t += step)
    {
        glm::vec2 p0 = parametric(t);
        glm::vec2 p1 = parametric(t + step);
        utils::draw_line(p0, p1, 0.005f, color);
    }
}

glm::vec2 heart(float t)
{
    float angle = t * 2.0f * std::numbers::pi;
    float x = 0.36f * std::sin(angle) * std::sin(angle) * std::sin(angle);
    float y = 0.33f * std::cos(angle)
              - 0.05f * std::cos(2 * angle)
              - 0.02f * std::cos(3 * angle)
              - 0.01f * std::cos(4 * angle);
    return glm::vec2{x, y};
}

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    std::vector<Particle> particles;
    for (int i = 0; i < 1000; ++i)
    {
        particles.emplace_back();
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        float dt = gl::delta_time_in_seconds();

        for (auto& p : particles)
        {
            p.age += dt;
            glm::vec2 old_pos = p.position;
            glm::vec2 new_pos = p.position + p.velocity * dt;
            glm::vec2 hit_point;
            bool hit = false;

            // particles do not die
            glm::vec4 color = p.color_start;
            float radius = 0.01f;
        }

        draw_parametric(heart);
    }
}