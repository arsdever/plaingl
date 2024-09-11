import gamify


class camera_movement():
    def __init__(self):
        pass

    def on_update(self):
        self.transform.rotate(self.transform.get_right(), 0.01)


def register():
    map = {"camera_movement":
           {camera_movement.__init__, camera_movement.on_update}}
    return map
