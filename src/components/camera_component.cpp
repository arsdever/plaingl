#include "components/camera_component.hpp"

#include "camera.hpp"
#include "gizmo_drawer.hpp"

camera_component::camera_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void camera_component::set_camera(camera* m) { _camera = m; }

camera* camera_component::get_camera() { return _camera; }

void camera_component::update()
{
    _camera->get_transform() = get_game_object()->get_transform();
}

void camera_component::draw_gizmos()
{
    // https://docs.unity3d.com/Manual/FrustumSizeAtDistance.html
    float distance = 20.0f;
    double frustumHeight = 2.0 * distance * std::tan(_camera->get_fov() * 0.5);
    double frustumWidth = frustumHeight * _camera->get_aspect_ratio();

    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 },
        { frustumWidth / 2, frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 },
        { -frustumWidth / 2, frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 },
        { -frustumWidth / 2, -frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });
    gizmo_drawer::instance()->draw_line(
        { 0, 0, 0 },
        { frustumWidth / 2, -frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });

    gizmo_drawer::instance()->draw_line(
        { frustumWidth / 2, frustumHeight / 2, -distance },
        { -frustumWidth / 2, frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });
    gizmo_drawer::instance()->draw_line(
        { -frustumWidth / 2, frustumHeight / 2, -distance },
        { -frustumWidth / 2, -frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });
    gizmo_drawer::instance()->draw_line(
        { -frustumWidth / 2, -frustumHeight / 2, -distance },
        { frustumWidth / 2, -frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });
    gizmo_drawer::instance()->draw_line(
        { frustumWidth / 2, -frustumHeight / 2, -distance },
        { frustumWidth / 2, frustumHeight / 2, -distance },
        { 1.0f, 1.0f, 0.0f, 1.0f });
}
