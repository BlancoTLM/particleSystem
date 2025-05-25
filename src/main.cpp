#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <glm/glm.hpp>

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    float mass;

    Particle() {
        position = glm::vec2{
            utils::rand(-gl::window_aspect_ratio(), +gl::window_aspect_ratio()),
            utils::rand(-1.0f, 1.0f)
        };

        float angle = 0.0f;//utils::rand(0.0f, 360.0f);
        float speed = 0.0f;//utils::rand(0.2f, 1.0f);

        velocity = glm::vec2{
            std::cos(angle) * speed,
            std::sin(angle) * speed
        };

        mass = utils::rand(0.0f, 2.0f);
    }
};

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    std::vector<Particle> particles;
    for (int i = 0; i < 100; ++i)
    {
        particles.emplace_back();
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        float dt = gl::delta_time_in_seconds();
        const glm::vec2 gravity = glm::vec2(0.0f, -0.1f);

        for (auto& p : particles)
        {
            glm::vec2 force = gravity * p.mass;
            glm::vec2 acceleration = force / p.mass;

            p.velocity += acceleration * dt;
            p.position += p.velocity * dt;

            utils::draw_disk(p.position, 0.02f, {1.0f, 0.5f, 0.2f, 1.0f});
        }
    }
}