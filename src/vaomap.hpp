#pragma once

#include "context_aware_object.hpp"

struct GLFWwindow;

class vao_map : public context_aware_object<unsigned, GLFWwindow*>
{
public:
    vao_map() = default;
    vao_map(const vao_map& other) = delete;
    vao_map(vao_map&& other) = default;
    vao_map& operator=(const vao_map& other) = delete;
    vao_map& operator=(vao_map&& other) = default;
    ~vao_map() override;

    bool activate() override;
};
