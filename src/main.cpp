#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <array>
#include <numbers>
#include <glm/glm.hpp>

struct Particle {

    glm::vec2 position;
    glm::vec2 velocity;
    float age = 0.0f;
    glm::vec4 color_start;
};

void draw_parametric(std::function<glm::vec2(float)> const& parametric)
{
    const float step = 0.01f;
    glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

    for (float t = 0.0f; t < 1.0f; t += step)
    {
        glm::vec2 p0 = parametric(t);
        glm::vec2 p1 = parametric(t + step);
        utils::draw_line(p0, p1, 0.01f, color);
    }
}

glm::vec2 lerp(glm::vec2 a, glm::vec2 b, float t)
{
    return (1.0f - t) * a + t * b;
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

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec2 p0 = {-0.8f, 0.0f};
    glm::vec2 p1 = {-0.4f, 0.8f};
    glm::vec2 p2 = { 0.4f, -0.8f};
    glm::vec2 p3 = { 0.8f, 0.0f};

    std::vector<Particle> particles;
   const int particle_count = 100;
for (int i = 0; i < particle_count; ++i)
{
    float t = static_cast<float>(i) / (particle_count - 1); // Échantillons réguliers entre 0 et 1
    glm::vec2 pos = bezier3_decasteljau(p0, p1, p2, p3, t);

    Particle p;
    p.position = pos;
    p.velocity = glm::vec2(0.0f);
    p.color_start = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // rouge
    particles.push_back(p);
}

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        float dt = gl::delta_time_in_seconds();

        draw_parametric([&](float t) {
            return bezier3_decasteljau(p0, p1, p2, p3, t);
        });

        for (auto& p : particles)
        {
            utils::draw_disk(p.position, 0.01f, p.color_start);
        }
    }
}