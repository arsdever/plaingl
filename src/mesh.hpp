#pragma once

#include <unordered_map>

struct GLFWwindow;

class mesh
{
public:
    mesh();
    mesh(mesh&& m);
    mesh(const mesh& m) = delete;
    mesh& operator=(mesh&& m);
    mesh& operator=(const mesh& m) = delete;
    ~mesh();

    void init();

    // TODO: not the best approach
    void render();

private:
    unsigned int _vao = 0;
    std::unordered_map<GLFWwindow*, unsigned int> _vao_map;
    unsigned int _vbo = 0;
    unsigned int _ebo = 0;
};
