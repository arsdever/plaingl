#include "polygon_to_mesh.hpp"

// For the given polygon generates a mesh
void polygon_to_mesh(std::vector<glm::vec2> pts,
                     bool closed,
                     float thickness,
                     std::function<void(glm::vec2)> vertex_func,
                     std::function<void(unsigned)> index_func)
{
    int numPts = pts.size();
    for (int i = 0; i < numPts - 1 + closed; ++i)
    // for (int i = 0; i < 1; ++i)
    {
        int a, b, c, d;
        b = i;
        c = i + 1;
        if (closed)
        {
            a = (i - 1 + numPts) % numPts;
            c = c % numPts;
            d = (i + 2) % numPts;
        }
        else
        {
            a = std::max(i - 1, 0);
            d = std::min(i + 2, numPts - 1);
        }

        auto p0 = pts[ a ];
        auto p1 = pts[ b ];
        auto p2 = pts[ c ];
        auto p3 = pts[ d ];
        auto line = glm::normalize(p2 - p1);
        auto normal = glm::normalize(glm::vec2(-line.y, line.x));
        auto tangent1 =
            (p0 == p1) ? line : glm::normalize(glm::normalize(p1 - p0) + line);
        auto tangent2 =
            (p2 == p3) ? line : glm::normalize(glm::normalize(p3 - p2) + line);

        auto miter1 = glm::vec2(-tangent1.y, tangent1.x);
        float length1 = thickness / glm::dot(normal, miter1);

        vertex_func(p1 - length1 * miter1);
        vertex_func(p1 + length1 * miter1);

        if (!closed && i == numPts - 2)
        {
            auto miter2 = glm::vec2(-tangent2.y, tangent2.x);
            float length2 = thickness / glm::dot(normal, miter2);
            vertex_func(p2 - length2 * miter2);
            vertex_func(p2 + length2 * miter2);
        }

        if (i == numPts - 1)
        {
            index_func(i * 2);
            index_func(i * 2 + 1);
            index_func(0);
            index_func(i * 2 + 1);
            index_func(0);
            index_func(1);
        }
        else
        {
            index_func(i * 2);
            index_func(i * 2 + 1);
            index_func(i * 2 + 2);
            index_func(i * 2 + 1);
            index_func(i * 2 + 2);
            index_func(i * 2 + 3);
        }
    }
}
