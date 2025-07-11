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

        mass = utils::rand(0.0f, 2.0f);
        lifetime = utils::rand(5.0f, 10.0f);

        color_start = glm::vec4
        {
            1.0f,
            1.0f,
            1.0f,
            1.0f
        };

        color_end = glm::vec4
        {
            1.0f,
            1.0f,
            1.0f,
            1.0f
        };
    }
};

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

            utils::draw_disk(p.position, radius, color);
        }
    }
}