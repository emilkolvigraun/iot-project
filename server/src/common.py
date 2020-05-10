
PUBLISH: int = 0
SUBSCRIBE: int = 1

class Common:

    sensors: dict = {}
    updates: list = [()]

    rooms: dict = {}

    def __init__(self):
        pass

    def update(self, action:int, topic:str, data:str=None):
        self.updates.append((action, topic, data))

    def number_of_updates(self):
        return len(self.updates)

    def tasks(self):
        tasks = []
        if len(self.updates) < 2:
            return tasks
        
        tasks = self.updates[1:]
        self.updates = [self.updates[0]]
        return tasks

    def register_sensor(self, key:str):
        if key not in self.sensors.keys():
            self.sensors.update({key:{}})

            self.update(SUBSCRIBE, key+'/temperature')
            self.update(SUBSCRIBE, key+'/humidity')
            self.update(SUBSCRIBE, key+'/lux')

    def stop(self):
        self.updates.clear()

    def append_room(self, room:dict):
        name = room['room']
        self.rooms.update({name:{
            'sensor':room['sensor'], 
            'windowHeight': room['windowHeight'], 
            'windowWidth': room['windowWidth'],
            'roomHeight': room['roomHeight'],
            'roomWidth': room['roomWidth'],
            'roomLength': room['roomLength'],
            'nrOfWindows': room['nrOfWindows'],
            'tDay': room['tDay'],
            'tNight': room['tNight'],
            'ventilation': room['ventilation']}})