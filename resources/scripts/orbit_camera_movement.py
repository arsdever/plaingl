import gamify
from gamify import python_component as pycomponent
from gamify import input_system as input


class orbit_camera_movement(pycomponent):
    name = "orbit_camera_movement"
    rotation = gamify.vec2(0.0, 0.0)
    distance = 10

    def __init__(self, gobj):
        pycomponent.__init__(self, self.name, gobj)

    def on_init(self):
        pass

    def on_update(self):
        tr = self.transform()

        if input.mouse_button(input.mouse_button_code.MouseButtonLeft) == input.button_state.Press:
            self.rotation.x -= input.mouse_delta().y * 0.01
            self.rotation.y -= input.mouse_delta().x * 0.01

        tr.set_rotation(gamify.vec3(self.rotation.x, self.rotation.y, 0))
        tr.set_position(tr.forward() * self.distance)

    def on_deinit(self):
        pass


module_exports = {orbit_camera_movement}
