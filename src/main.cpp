#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <glm/glm.hpp>

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;

    Particle() {
        position = glm::vec2{
            utils::rand(-gl::window_aspect_ratio(), +gl::window_aspect_ratio()),
            utils::rand(-1.0f, 1.0f)
        };

        float angle = utils::rand(0.0f, 360.0f);
        float speed = utils::rand(0.2f, 1.0f);

        velocity = glm::vec2{
            std::cos(angle) * speed,
            std::sin(angle) * speed
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
    for (int i = 0; i < 100; ++i) {
        particles.emplace_back();
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (const auto& p : particles) {
            utils::draw_disk(p.position, 0.02f, {1.0f, 0.5f, 0.2f, 1.0f}); // rayon fixe, couleur orange
        }

        float dt = gl::delta_time_in_seconds();

        for (auto& p : particles) {
        p.position += p.velocity * dt;

        utils::draw_disk(p.position, 0.02f, {1.0f, 0.5f, 0.2f, 1.0f});
        }
    }
}