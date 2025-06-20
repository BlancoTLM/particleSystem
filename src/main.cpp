#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <array>
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

        float angle = glm::radians(utils::rand(0.0f, 360.0f));
        float speed = utils::rand(0.1f, 0.2f);

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

bool segment_intersect(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& q1,
    const glm::vec2& q2, glm::vec2& intersection)
{
    glm::vec2 r = p2 - p1;
    glm::vec2 s = q2 - q1;

    float rxs = r.x * s.y - r.y * s.x;
    float qpxr = (q1 - p1).x * r.y - (q1 - p1).y * r.x;

    if (rxs == 0.0f)
    return false;

    float t = ((q1 - p1).x * s.y - (q1 - p1).y * s.x) / rxs;
    float u = ((q1 - p1).x * r.y - (q1 - p1).y * r.x) / rxs;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        intersection = p1 + t * r;
        return true;
    }

    return false;
}

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    std::array<glm::vec2, 5> star_points = {
        glm::vec2{ 0.0f,  0.5f},
        glm::vec2{ 0.4755f, 0.1545f},
        glm::vec2{ 0.2939f, -0.4045f},
        glm::vec2{-0.2939f, -0.4045f},
        glm::vec2{-0.4755f, 0.1545f}
    };

    std::array<std::pair<int, int>, 5> star_segments = {
        std::make_pair(0, 2),
        std::make_pair(2, 4),
        std::make_pair(4, 1),
        std::make_pair(1, 3),
        std::make_pair(3, 0)
    };

    std::vector<Particle> particles;
    for (int i = 0; i < 100; ++i)
    {
        particles.emplace_back();
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 mouse = gl::mouse_position();

        glm::vec2 line_start = glm::vec2(0.0f, -1.0f);  // centre bas
        glm::vec2 line_end = mouse;

        utils::draw_line(line_start, line_end, 0.005f, glm::vec4(1, 0, 0, 1));
            
        for (const auto& seg : star_segments)
        {
            glm::vec2 start = star_points[seg.first];
            glm::vec2 end = star_points[seg.second];
            glm::vec2 hit_point;
            if (segment_intersect(line_start, line_end, start, end, hit_point))
            {
                utils::draw_disk(hit_point, 0.02f, glm::vec4(1, 0, 1, 1)); // magenta pour différencier
            }
        }

        for (auto const& seg : star_segments)
        {
            glm::vec2 start = star_points[seg.first];
            glm::vec2 end = star_points[seg.second];
            utils::draw_line(start, end, 0.01f, glm::vec4(1.f, 1.f, 1.f, 1.f));
        }

        float dt = gl::delta_time_in_seconds();
        // particles.erase(
        //     std::remove_if(particles.begin(), particles.end(), [](const Particle& p)
        //     {return p.age >= p.lifetime;}),
        //     particles.end()
        // );

        for (auto& p : particles)
        {
            p.age += dt;
            glm::vec2 old_pos = p.position;
            glm::vec2 new_pos = p.position + p.velocity * dt;
            glm::vec2 hit_point;

            for (auto& p : particles)
            {
                p.age += dt;
                p.position += p.velocity * dt * 0.03f;
            }

            // particle lifespan
            // float raw_t = glm::clamp((p.age / p.lifetime) * 3.0f, 0.0f, 1.0f);
            // float t = easeInOutPower(raw_t, 2.0f);
            // glm::vec4 color = glm::mix(p.color_start, p.color_end, t);

            // float fade_time = 2.0f;
            // float time_left = p.lifetime - p.age;
            
            // float mask = glm::clamp((fade_time - time_left) / fade_time, 0.0f, 1.0f);
            // float base_radius = 0.1f * (1.0f - (p.age / p.lifetime));
            // float amplitude = 1.5f;
            // float radius = base_radius * (1.0f - mask) + base_radius * mask * (1.0f + amplitude * (bounce(time_left / fade_time) - 0.5f));

            // particles do not die
            glm::vec4 color = p.color_start;
            float radius = 0.05f;

            utils::draw_disk(p.position, radius, color);
        }
    }
}