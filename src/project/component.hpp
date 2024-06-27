#pragma once

#include "project/object.hpp"

class game_object;

namespace components
{
class transform;
} // namespace components

class component : public object
{
public:
    game_object& get_game_object() const;

    components::transform& get_transform() const;

    void set_active(bool active = true);
    bool is_active() const;

    void init();
    void update();
    void deinit();

protected:
    component(std::string_view name, game_object& obj);

    virtual void on_init();
    virtual void on_update();
    virtual void on_deinit();

    std::reference_wrapper<game_object> _game_object;

private:
    bool _is_active { true };
};
