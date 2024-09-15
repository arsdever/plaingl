import gamify


class camera_movement(gamify.python_component):
    name = "camera_movement"

    def __init__(self, gobj):
        gamify.python_component.__init__(self, self.name, gobj)

    def on_init(self):
        pass

    def on_update(self):
        tr = self.transform()
        right = tr.get_right()
        tr.rotate(right, 0.1)

    def on_deinit(self):
        pass


module_exports = {camera_movement}
