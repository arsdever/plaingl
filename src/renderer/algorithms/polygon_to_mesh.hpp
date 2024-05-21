#pragma once

// For the given polygon generates a mesh
void polygon_to_mesh(std::vector<glm::vec2> pts,
                     bool closed,
                     float thickness,
                     std::function<void(glm::vec2)> vertex_func,
                     std::function<void(unsigned)> index_func);
