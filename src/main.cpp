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

        for (auto const& seg : star_segments)
        {
            glm::vec2 start = star_points[seg.first];
            glm::vec2 end = star_points[seg.second];
            utils::draw_line(start, end, 0.01f, glm::vec4(1.f, 1.f, 1.f, 1.f));
        }

        float dt = gl::delta_time_in_seconds();

        for (auto& p : particles)
        {
            p.age += dt;
            glm::vec2 old_pos = p.position;
            glm::vec2 new_pos = p.position + p.velocity * dt;
            glm::vec2 hit_point;
            bool hit = false;

            for (auto const& seg : star_segments)
            {
                glm::vec2 start = star_points[seg.first];
                glm::vec2 end = star_points[seg.second];

                if (segment_intersect(old_pos, new_pos, start, end, hit_point))
                {
                    hit = true;
                    glm::vec2 wall_dir = glm::normalize(end - start);
                    glm::vec2 wall_normal = glm::vec2(-wall_dir.y, wall_dir.x);

                    p.velocity = p.velocity - 2.0f * glm::dot(p.velocity, wall_normal) * wall_normal;

                    float distance_behind = glm::length(new_pos - hit_point);
                    p.position = hit_point + glm::normalize(p.velocity) * distance_behind;

                    break;
                }
            }

            if (!hit)
            {
                p.position = new_pos;
            }

            // particles do not die
            glm::vec4 color = p.color_start;
            float radius = 0.05f;

            utils::draw_disk(p.position, radius, color);
        }
    }
}