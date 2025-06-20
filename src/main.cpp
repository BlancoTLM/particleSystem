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

bool segment_circle_intersect(
    const glm::vec2& p1,
    const glm::vec2& p2,
    const glm::vec2& circle_center,
    float circle_radius,
    glm::vec2& intersection)
{
    glm::vec2 d = p2 - p1;
    glm::vec2 f = p1 - circle_center;

    float a = glm::dot(d, d);
    float b = 2.0f * glm::dot(f, d);
    float c = glm::dot(f, f) - circle_radius * circle_radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0.0f) {
        return false;
    }

    discriminant = std::sqrt(discriminant);

    float t1 = (-b - discriminant) / (2 * a);
    float t2 = (-b + discriminant) / (2 * a);

    bool hit = false;
    float t = 0.0f;

    if (t1 >= 0.0f && t1 <= 1.0f) {
        t = t1;
        hit = true;
    } else if (t2 >= 0.0f && t2 <= 1.0f) {
        t = t2;
        hit = true;
    }

    if (hit) {
        intersection = p1 + t * d;
    }

    return hit;
}

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    std::array<glm::vec2, 9> star_points = {
    glm::vec2{ 0.0f,  0.5f},
    glm::vec2{ 0.4755f, 0.1545f},
    glm::vec2{ 0.2939f, -0.4045f},
    glm::vec2{-0.2939f, -0.4045f},
    glm::vec2{-0.4755f, 0.1545f}, 
    {-gl::window_aspect_ratio(), -1.0f}, 
    { gl::window_aspect_ratio(), -1.0f},
    { gl::window_aspect_ratio(),  1.0f}, 
    {-gl::window_aspect_ratio(),  1.0f}  
    };

    std::array<std::pair<int, int>, 9> star_segments = {
        std::make_pair(0, 2),
        std::make_pair(2, 4),
        std::make_pair(4, 1),
        std::make_pair(1, 3),
        std::make_pair(3, 0),

        std::make_pair(5, 6),
        std::make_pair(6, 7),
        std::make_pair(7, 8),
        std::make_pair(8, 5)
    };

    std::vector<std::pair<glm::vec2, float>> circle_obstacles =
    {
        { glm::vec2(-0.5f, 0.0f), 0.15f },
        { glm::vec2( 0.5f, 0.4f), 0.1f },
        { glm::vec2( 0.0f, -0.5f), 0.2f }
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

            if (!hit) {
    for (auto const& circle : circle_obstacles)
    {
        glm::vec2 hit_point;
        if (segment_circle_intersect(old_pos, new_pos, circle.first, circle.second, hit_point))
        {
            hit = true;

            glm::vec2 normal = glm::normalize(hit_point - circle.first);
            p.velocity = p.velocity - 2.0f * glm::dot(p.velocity, normal) * normal;

            float distance_behind = glm::length(new_pos - hit_point);
            p.position = hit_point + glm::normalize(p.velocity) * distance_behind;

            break;
        }
    }
}

            for (auto const& circle : circle_obstacles)
            {
                utils::draw_disk(circle.first, circle.second, glm::vec4(1, 0, 0, 0.3f));
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