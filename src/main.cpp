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

        mass = utils::rand(0.0f, 2.0f);
        lifetime = utils::rand(1.0f, 5.0f);
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
        // const glm::vec2 gravity = glm::vec2(0.0f, -0.1f);

        // glm::vec2 mouse_pos = gl::mouse_position();
        // float spring_k = 5.0f;
        // float damping_d = 5.0f;
        // float max_distance = 0.5f;

        particles.erase(
            std::remove_if(particles.begin(), particles.end(), [](const Particle& p)
            {return p.age >= p.lifetime;}),
            particles.end()
        );

        for (auto& p : particles)
        {
            // GRAVITY
            // glm::vec2 force = gravity * p.mass;
            // glm::vec2 acceleration = force / p.mass;
            // p.velocity += acceleration * dt;

            // SPRING
            // glm::vec2 spring_force = -spring_k * (p.position - mouse_pos);
            // glm::vec2 damping_force = -damping_d * p.velocity;
            // glm::vec2 total_force = spring_force + damping_force;
            // glm::vec2 acceleration = spring_force / p.mass;
            // p.velocity += acceleration * dt;
            // glm::vec2 new_pos = p.position + p.velocity * dt;
            // glm::vec2 dir = new_pos - mouse_pos;
            // float dist = glm::length(dir);

            // if (dist <= max_distance)
            // {
            //     p.position = new_pos;
            // }
            // else
            // {
            //     glm::vec2 from_mouse = glm::normalize(p.position - mouse_pos);
            //     glm::vec2 radial_velocity = glm::dot(p.velocity, from_mouse) * from_mouse;
            //     glm::vec2 tangential_velocity = p.velocity - radial_velocity;
            //
            //     if (glm::dot(radial_velocity, from_mouse) < 0)
            //     {
            //         p.position = new_pos;
            //     }
            //     else
            //     {
            //         p.velocity = tangential_velocity;
            //         p.position += p.velocity * dt;
            //     }
            // }

            p.age += dt;
            p.position += p.velocity * dt;

            utils::draw_disk(p.position, 0.02f, {1.0f, 0.5f, 0.2f, 1.0f});
        }
    }
}