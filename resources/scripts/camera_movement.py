import gamify
from gamify import python_component as pycomponent
from gamify import input_system as input


class camera_movement(pycomponent):
    name = "camera_movement"

    def __init__(self, gobj):
        pycomponent.__init__(self, self.name, gobj)

    def on_init(self):
        pass

    def on_update(self):
        tr = self.transform()
        right = tr.get_right()
        tr.rotate(right, 0.1)

        print(input.mouse_button(input.mouse_button_code.MouseButtonLeft))
        if input.mouse_button(input.mouse_button_code.MouseButtonLeft) == input.button_state.Hold:
            print("{}x{}", input.mouse_delta().x, input.mouse_delta().y)

    def on_deinit(self):
        pass


module_exports = {camera_movement}
