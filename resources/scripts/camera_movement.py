import gamify


class camera_movement(gamify.python_component):
    name = "camera_movement"

    def __init__(self, gobj):
        gamify.python_component.__init__(self, gobj)
        self.name = "hello"

    def on_init(self):
        pass

    def on_update(self):
        print("Updating")
        tr = self.transform()
        print(type(tr))
        right = tr.get_right()
        print(type(right))
        tr.rotate(right, 0.1)
        self.name = "world"

    def on_deinit(self):
        pass


module_exports = {camera_movement}
