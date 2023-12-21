#pragma once

#include <memory>
#include <string_view>

#include "game_object.hpp"

namespace spdlog
{
class logger;
} // namespace spdlog

using logger = std::shared_ptr<spdlog::logger>;

class component
{
public:
    component(game_object* parent, std::string_view type_id);

    virtual ~component();

    std::string_view type_id() const;

    virtual void init();

    virtual void update();

    virtual void draw_gizmos();

    virtual void deinit();

    logger log() const;

    game_object* get_game_object() const;

    template <typename T>
    T* get_component()
    {
        return _parent->get_component<T>();
    }

    static constexpr std::string_view class_type_id = "component";

private:
    game_object* _parent;
    std::string_view _type_id;
};
