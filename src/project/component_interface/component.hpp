#pragma once

#include "project/project_fwd.hpp"

#include "project/game_object.hpp"
#include "project/object.hpp"

class component : public object
{
public:
    game_object& get_game_object() const;

    components::transform& get_transform() const;

    inline void enable() { set_enabled(true); }
    inline void disable() { set_enabled(false); }
    void set_enabled(bool active = true);
    bool is_enabled() const;

    void init();
    void update();
    void deinit();

    static size_t type_id(std::string_view class_name);
    static constexpr std::string_view type_name = "component";

    template <typename T>
    static size_t type_id()
    {
        return type_id(T::type_name);
    }

protected:
    component(std::string_view name, game_object& obj);

    virtual void on_init();
    virtual void on_update();
    virtual void on_deinit();

    std::reference_wrapper<game_object> _game_object;

private:
    bool _is_enabled { true };
};
