#include "scene.hpp"

scene::scene() { _scene_instance = this; }

const std::vector<game_object*>& scene::objects() const { return _objects; }

void scene::add_object(game_object* object) { _objects.push_back(object); }

scene* scene::get_active_scene() { return _scene_instance; }

scene* scene::_scene_instance = nullptr;
