#pragma once

#include "component.hpp"

class camera;

class renderer : public component
{
public:
    virtual ~renderer() = 0;
    virtual void render() = 0;
};

inline renderer::~renderer() = default;
