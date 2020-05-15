import json

PUBLISH: int = 0
SUBSCRIBE: int = 1

class Common:

    sensors: dict = {}
    updates: list = [()]
    rooms: dict = {}

    selected_room: str = ''

    def __init__(self):
        try:
            with open('../etc/rooms.json') as cf:
                self.rooms = json.loads(''.join(cf.readlines()))
            with open('../etc/sensors.json') as cf:
                self.sensors = json.loads(''.join(cf.readlines()))
        except:
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

    def register_sensor(self, msg:str):
        self.update_sensors(msg.split('/')[0])
        self.update(PUBLISH, 'archiver/subscribe', msg)

    def update_sensors(self, key:str):
        if key not in self.sensors.keys():
            self.sensors.update({key:{}})
            
            with open('../etc/sensors.json', 'w') as cf:
                cf.write(json.dumps(self.sensors))


    def stop(self):
        self.updates.clear()

    def append_room(self, room:dict):
        name = room['room']
        self.rooms.update({name:{
            'sensor':room['sensor'], 
            'roomHeight': room['roomHeight'],
            'roomWidth': room['roomWidth'],
            'roomLength': room['roomLength'],
            'nrOfWindows': room['nrOfWindows'],
            'tDay': room['tDay'],
            'tNight': room['tNight'],
            'ventilation': room['ventilation']}})

        print(self.rooms)
        with open('../etc/rooms.json', 'w') as cf:
            cf.write(json.dumps(self.rooms))
