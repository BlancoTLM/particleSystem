#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <array>
#include <numbers>
#include <glm/glm.hpp>

struct Particle
{

    glm::vec2 position;
    glm::vec2 velocity;
    float age = 0.0f;
    glm::vec4 color_start;
};

void draw_parametric(std::function<glm::vec2(float)> const &parametric)
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

glm::vec2 bezier3_derivative(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
    const float h = 1e-4f;
    glm::vec2 before = bezier3_decasteljau(p0, p1, p2, p3, glm::clamp(t - h, 0.0f, 1.0f));
    glm::vec2 after = bezier3_decasteljau(p0, p1, p2, p3, glm::clamp(t + h, 0.0f, 1.0f));
    return (after - before) / (2.0f * h);
}

float find_closest_t_gradient_descent(glm::vec2 point,
                                      glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
    float t = 0.5f;
    const float learning_rate = 0.01f;
    const int max_iterations = 100;
    const float epsilon = 1e-5f;

    for (int i = 0; i < max_iterations; ++i)
    {
        glm::vec2 b = bezier3_decasteljau(p0, p1, p2, p3, t);
        glm::vec2 b_deriv = bezier3_derivative(p0, p1, p2, p3, t);

        float gradient = 2.0f * glm::dot(b - point, b_deriv);

        t -= learning_rate * gradient;

        t = glm::clamp(t, 0.0f, 1.0f);

        if (std::abs(gradient) < epsilon)
            break;
    }

    return t;
};

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec2 p0 = {-0.8f, 0.0f};
    glm::vec2 p1 = {-0.4f, 0.8f};
    glm::vec2 p2 = {0.4f, -0.8f};
    glm::vec2 p3 = {0.8f, 0.0f};

    std::vector<Particle> particles;
    const int particle_count = 100;
    int rows = 4;
    int cols = 25;
    float row_spacing = 0.03f;

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            float t = static_cast<float>(col) / (cols - 1);
            glm::vec2 pos_on_curve = bezier3_decasteljau(p0, p1, p2, p3, t);

            glm::vec2 tangent = bezier3_derivative(p0, p1, p2, p3, t);
            tangent = glm::normalize(tangent);

            glm::vec2 normal = glm::vec2(-tangent.y, tangent.x);
            if (normal.y < 0)
                normal = -normal;

            float offset_distance = 0.1f + row * row_spacing;
            glm::vec2 offset = normal * offset_distance;

            Particle p;
            p.position = pos_on_curve + offset;
            p.velocity = glm::vec2(0.0f);
            p.color_start = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // rouge

            particles.push_back(p);
        }
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        float dt = gl::delta_time_in_seconds();

        draw_parametric([&](float t)
                        { return bezier3_decasteljau(p0, p1, p2, p3, t); });

        for (auto &p : particles)
        {
            glm::vec2 gravity = glm::vec2(0.0f, -1.0f);
            p.velocity += gravity * dt;

            float t_closest = find_closest_t_gradient_descent(p.position, p0, p1, p2, p3);
            glm::vec2 curve_point = bezier3_decasteljau(p0, p1, p2, p3, t_closest);

            const float delta = 0.001f;
            glm::vec2 before = bezier3_decasteljau(p0, p1, p2, p3, glm::clamp(t_closest - delta, 0.f, 1.f));
            glm::vec2 after = bezier3_decasteljau(p0, p1, p2, p3, glm::clamp(t_closest + delta, 0.f, 1.f));
            glm::vec2 tangent = glm::normalize(after - before);
            glm::vec2 normal = glm::vec2(-tangent.y, tangent.x);

            if (normal.y < 0)
                normal = -normal;

            glm::vec2 to_curve = curve_point - p.position;
            float distance = glm::length(to_curve);

            if (distance < 0.02f)
            {
                float strength = 10.0f * std::exp(-20.0f * distance);
                glm::vec2 bounce_force = normal * strength;

                p.velocity += bounce_force * dt;
            }

            p.velocity *= 0.99f;

            p.position += p.velocity * dt;

            utils::draw_disk(p.position, 0.01f, p.color_start);
        }
    }
};