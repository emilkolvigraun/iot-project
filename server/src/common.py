
PUBLISH: int = 0
SUBSCRIBE: int = 1

class Common:

    selected_sensor: str = ''
    sensors: dict = {}
    updates: list = [()]

    def __init__(self):
        pass

    def update(self, action:int, topic:str, data:str=None):
        self.updates.append((action, topic, data))

    def select_sensor(self, key:str):
        self.selected_sensor = key

    def register_sensor(self, key:str):
        if key not in self.sensors.keys():
            self.sensors.update({key:{}})

            self.update(SUBSCRIBE, key+'/temperature')
            self.update(SUBSCRIBE, key+'/humidity')
            self.update(SUBSCRIBE, key+'/lux')