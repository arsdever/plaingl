#pragma once

#include "common/event.hpp"
#include "project/object.hpp"

class game_object;

class scene : public object
{
public:
    static std::shared_ptr<scene> create();
    static std::shared_ptr<scene> load(std::string_view path);

    void add_root_object(std::shared_ptr<game_object> object);
    bool visit_root_objects(
        std::function<bool(const std::shared_ptr<game_object>&)> visitor) const;

    void save(std::string_view path);
    void unload();

    event<bool()> save_request;


private:
    scene();

    std::vector<std::shared_ptr<game_object>> _root_objects;
    static std::shared_ptr<scene> _active_scene;

    friend class memory_manager;
};
