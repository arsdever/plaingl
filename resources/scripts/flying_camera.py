import gamify
from gamify import python_component as pycomponent
from gamify import input_system as input
from gamify import vec2 as vec2
from gamify import vec3 as vec3


class flying_camera(pycomponent):
    name = "flying_camera"
    up = None
    down = None
    move = None
    rotate = None
    movement_speed = .1
    rotation_speed = .01
    rotation = vec2(0, 0)

    def __init__(self, gobj):
        pycomponent.__init__(self, self.name, gobj)

    def on_init(self):
        self.up = input.bind("gamepad.left_trigger", "up")
        self.down = input.bind("gamepad.right_trigger", "down")
        self.move = input.bind("gamepad.left_joystick", "move")
        self.rotate = input.bind("gamepad.right_joystick", "rotate")

    def on_update(self):
        tr = self.transform()
        diff = self.move.get_vec2()
        updown = self.up.get_float() - self.down.get_float()
        movement = (tr.right() * diff.x + tr.forward()
                    * diff.y + tr.up() * updown) * self.movement_speed
        tr.move(movement)

        rot_diff = self.rotate.get_vec2()
        self.rotation.x -= rot_diff.x * self.rotation_speed
        self.rotation.y -= rot_diff.y * self.rotation_speed
        tr.set_rotation(vec3(self.rotation.y, self.rotation.x, 0))

    def on_deinit(self):
        pass


module_exports = {flying_camera}
