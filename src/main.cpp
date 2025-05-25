#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
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
        position = glm::vec2{
            utils::rand(-gl::window_aspect_ratio(), +gl::window_aspect_ratio()),
            utils::rand(-1.0f, 1.0f)
        };

        float angle = utils::rand(0.0f, 360.0f);
        float speed = utils::rand(0.01f, 0.2f);

        velocity = glm::vec2{
            std::cos(angle) * speed,
            std::sin(angle) * speed
        };

        mass = utils::rand(0.0f, 2.0f);
        lifetime = utils::rand(5.0f, 10.0f);

        color_start = glm::vec4
        {
            utils::rand(0.5f, 1.0f),
            utils::rand(0.5f, 1.0f),
            utils::rand(0.5f, 1.0f),
            1.0f
        };

        color_end = glm::vec4
        {
            utils::rand(0.5f, 1.0f),
            utils::rand(0.5f, 1.0f),
            utils::rand(0.5f, 1.0f),
            1.0f
        };
    }
};

float bounce(float x) {
    return std::abs(std::sin(10.0f * 3.14f * x));
}

float easeInOutPower(float t, float power) {
    if (t < 0.5f)
        return 0.5f * std::pow(2.0f * t, power);
    else
        return 1.0f - 0.5f * std::pow(2.0f * (1.0f - t), power);
}

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
        particles.erase(
            std::remove_if(particles.begin(), particles.end(), [](const Particle& p)
            {return p.age >= p.lifetime;}),
            particles.end()
        );

        for (auto& p : particles)
        {
            p.age += dt;
            p.position += p.velocity * dt;

            float raw_t = glm::clamp((p.age / p.lifetime) * 3.0f, 0.0f, 1.0f);
            float t = easeInOutPower(raw_t, 2.0f);  // power=2 pour un easing doux
            glm::vec4 color = glm::mix(p.color_start, p.color_end, t);

            float fade_time = 2.0f;
            float time_left = p.lifetime - p.age;
            
            float mask = glm::clamp((fade_time - time_left) / fade_time, 0.0f, 1.0f);
            float base_radius = 0.1f * (1.0f - (p.age / p.lifetime));
            float amplitude = 1.5f;
            float radius = base_radius * (1.0f - mask) + base_radius * mask * (1.0f + amplitude * (bounce(time_left / fade_time) - 0.5f));

            utils::draw_disk(p.position, radius, color);
        }
    }
}