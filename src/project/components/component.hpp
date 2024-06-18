#pragma once

#include "project/object.hpp"

class game_object;

namespace components
{

class transform;

class component : public object
{
public:
    game_object& get_game_object() const;

    transform& get_transform();

    static size_t register_component();

    void set_active(bool active = true);
    bool is_active() const;

    void init();
    void update();
    void deinit();

protected:
    component(const std::string& name, game_object& obj);

    virtual void on_init();
    virtual void on_update();
    virtual void on_deinit();

    game_object& _game_object;
    transform& _transform;

private:
    bool _is_active { true };
};
} // namespace components
