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

glm::vec2 lerp(glm::vec2 a, glm::vec2 b, float t)
{
    return (1.0f - t) * a + t * b;
}

glm::vec2 bezier1_decasteljau(glm::vec2 p0, glm::vec2 p1, float t)
{
    return lerp(p0, p1, t);
}

glm::vec2 bezier1_bernstein(glm::vec2 p0, glm::vec2 p1, float t)
{
    return (1 - t) * p0 + t * p1;
}

glm::vec2 bezier2_decasteljau(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float t)
{
    glm::vec2 a = lerp(p0, p1, t);
    glm::vec2 b = lerp(p1, p2, t);
    return lerp(a, b, t);
}

glm::vec2 bezier2_bernstein(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float t)
{
    float u = 1 - t;
    return u * u * p0 + 2 * u * t * p1 + t * t * p2;
}

glm::vec2 bezier3_decasteljau(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
    glm::vec2 a = lerp(p0, p1, t);
    glm::vec2 b = lerp(p1, p2, t);
    glm::vec2 c = lerp(p2, p3, t);

    glm::vec2 d = lerp(a, b, t);
    glm::vec2 e = lerp(b, c, t);

    return lerp(d, e, t);
}

glm::vec2 bezier3_bernstein(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
    float u = 1 - t;
    return u * u * u * p0
         + 3 * u * u * t * p1
         + 3 * u * t * t * p2
         + t * t * t * p3;
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

        // draw_parametric([](float t) {
        //     return bezier1_decasteljau({-0.8f, -0.8f}, {0.8f, 0.8f}, t);
        // });

        // draw_parametric([](float t) {
        //     return bezier1_bernstein({-0.8f, -0.8f}, {0.8f, 0.8f}, t);
        // });

        // draw_parametric([](float t) {
        //     return bezier2_decasteljau({-0.8f, -0.5f}, {0.0f, 0.8f}, {0.8f, -0.5f}, t);
        // });

        // draw_parametric([](float t) {
        //     return bezier2_bernstein({-0.8f, -0.5f}, {0.0f, 0.8f}, {0.8f, -0.5f}, t);
        // });

        // draw_parametric([](float t) {
        //     return bezier3_decasteljau(
        // {-0.8f, 0.0f},
        // {-0.4f, 0.8f},
        // { 0.4f, -0.8f},
        // { 0.8f, 0.0f},
        // t
        //  );});

        // draw_parametric([](float t) {
        //     return bezier3_bernstein(
        // {-0.9f, 0.0f},
        // {-0.6f, 0.6f},
        // {-0.3f, -0.6f},
        // { 0.0f, 0.0f},
        // t
        // );});

        draw_parametric([](float t) {
            glm::vec2 mouse = gl::mouse_position();

           glm::vec2 p0 = {-0.9f, 0.0f};
           glm::vec2 p3 = { 0.9f, 0.0f};

          glm::vec2 p1 = glm::mix(p0, mouse, 1.0f);
            glm::vec2 p2 = glm::mix(p3, mouse, 1.0f);

           return bezier3_bernstein(p0, p1, p2, p3, t);
        });

    }
}