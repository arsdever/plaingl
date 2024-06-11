#pragma once

#include "project/object.hpp"

class game_object;

class scene : public object
{
public:
    static std::shared_ptr<scene> create();

    void add_object(std::shared_ptr<game_object> object);
    std::shared_ptr<game_object> create_object();

private:
    scene();

    std::vector<std::shared_ptr<game_object>> _objects;
};
