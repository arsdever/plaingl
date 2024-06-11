#include <glm/gtx/matrix_decompose.hpp>

#include "project/components/transform.hpp"

#include "project/memory_manager.hpp"

namespace components
{
transform::transform(game_object& obj)
    : component("Transform", obj)
{
}

transform& transform::create(game_object& obj)
{
    return memory_manager::create_component<transform>(obj);
}

transform& transform::get(const game_object& obj)
{
    return memory_manager::get_component<transform>(obj);
}

void transform::set_position(const glm::dvec3& position)
{
    _position = position;
    _dirty = true;
}

void transform::set_rotation(const glm::dquat& rotation)
{
    _rotation = rotation;
    _dirty = true;
}

void transform::set_scale(const glm::dvec3& scale)
{
    _scale = scale;
    _dirty = true;
}

template <>
glm::dmat4 transform::get_matrix<transform::relation_flag::local>() const
{
    recalculate_matrix();
    return _matrix;
}

template <>
glm::dmat4 transform::get_matrix<transform::relation_flag::world>() const
{
    recalculate_matrix();
    return _world_matrix;
}

template <>
glm::dvec3 transform::get_position<transform::relation_flag::local>() const
{
    return _position;
}

template <>
glm::dquat transform::get_rotation<transform::relation_flag::local>() const
{
    return _rotation;
}

template <>
glm::dvec3 transform::get_scale<transform::relation_flag::local>() const
{
    return _scale;
}

template <>
glm::dvec3 transform::get_position<transform::relation_flag::world>() const
{
    recalculate_matrix();
    return _world_position;
}

template <>
glm::dquat transform::get_rotation<transform::relation_flag::world>() const
{
    recalculate_matrix();
    return _world_rotation;
}

template <>
glm::dvec3 transform::get_scale<transform::relation_flag::world>() const
{
    recalculate_matrix();
    return _world_scale;
}

glm::dvec3 transform::get_forward() const
{
    return glm::normalize(glm::dvec3(get_matrix<relation_flag::local>() *
                                     glm::dvec4(0, 0, 1, 0)));
}

glm::dvec3 transform::get_right() const
{
    return glm::normalize(glm::dvec3(get_matrix<relation_flag::local>() *
                                     glm::dvec4(1, 0, 0, 0)));
}

glm::dvec3 transform::get_up() const
{
    return glm::normalize(glm::dvec3(get_matrix<relation_flag::local>() *
                                     glm::dvec4(0, 1, 0, 0)));
}

bool transform::is_dirty() const
{
    if (!_game_object.has_parent())
        return _dirty;

    return _game_object.get_parent()->get_transform().is_dirty();
}

bool transform::recalculate_matrix() const
{
    bool world_matrix_changed = false;
    if (_dirty)
    {
        _matrix = glm::identity<glm::dmat4>();
        _matrix = glm::translate(_matrix, _position) *
                  glm::mat4_cast(_rotation) * glm::scale(_matrix, _scale);
        world_matrix_changed = true;
        _dirty = false;
    }

    if (_game_object.has_parent())
    {
        if (_game_object.get_parent()->get_transform().recalculate_matrix() &&
            !world_matrix_changed)
        {
            return false;
        }
        _world_matrix =
            _game_object.get_parent()->get_transform().get_matrix() * _matrix;
    }
    else
    {
        if (world_matrix_changed)
            _world_matrix = _matrix;
    }

    if (world_matrix_changed)
    {
        glm::dvec3 skew;
        glm::dvec4 perspective;
        glm::decompose(_world_matrix,
                       _world_scale,
                       _world_rotation,
                       _world_position,
                       skew,
                       perspective);
    }

    return world_matrix_changed;
}
} // namespace components
