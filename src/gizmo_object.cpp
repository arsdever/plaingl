#include "gizmo_object.hpp"

#include "gizmo_drawer.hpp"

gizmo_object::gizmo_object() { }

void gizmo_object::update()
{
    if (!_line.has_value())
        return;

    gizmo_drawer::instance()->draw_line_2d(
        _line.value()[ 0 ], _line.value()[ 1 ], { 1, 0, 0, 1 });
}
