from gamify import input_system as input
from gamify import python_component as pycomponent
from gamify import dvec2 as vec2


class camera_input(pycomponent):
    name = "camera_input"

    up_bind = None
    down_bind = None
    move_bind = None
    rotate_bind = None

    updown = float()
    move = vec2()
    rotate = vec2()

    def __init__(self, gobj):
        pycomponent.__init__(self, self.name, gobj)

    def on_init(self):
        self.up_bind = input.bind("up", "gamepad.left_trigger")
        self.down_bind = input.bind("down", "gamepad.right_trigger")
        self.move_bind = input.bind("move", "gamepad.left_joystick")
        self.rotate_bind = input.bind("rotate", "gamepad.right_joystick")

    def on_update(self):
        self.updown = self.up_bind.get_float() - self.down_bind.get_float()
        self.move = self.move_bind.get_vec2()
        self.rotate = self.rotate_bind.get_vec2()

    def on_deinit(self):
        pass


module_exports = {camera_input}
