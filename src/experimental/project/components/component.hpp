#pragma once

#include "experimental/project/object.hpp"

class game_object;

namespace components
{

class transform;

class component : public object
{
public:
    virtual void on_start();
    virtual void on_update();
    virtual void on_destroy();

    game_object& get_game_object() const;

    transform& get_transform();

protected:
    component(const std::string& name, game_object& obj);

    game_object& _game_object;
    transform& _transform;
};
} // namespace components
