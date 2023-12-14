#pragma once

#include <vector>

// TODO: the implementation is very draft and needs redoing

class game_object;

class scene
{
public:
    scene();

    const std::vector<game_object*>& objects() const;
    void add_object(game_object* object);

    static scene* get_active_scene();

private:
    std::vector<game_object*> _objects;
    static scene* _scene_instance;
};
