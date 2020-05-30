import json

PUBLISH: int = 0
SUBSCRIBE: int = 1

class Common:

    sensors: dict = {}
    updates: list = [()]
    rooms: dict = {}
    alerts: list = []

    

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

    def jobs(self):
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

    def append_alert(self, msg:str):
        self.clean_alerts()
        self.alerts.append(msg)
            
    def get_alerts(self):
        self.clean_alerts()
        return self.alerts

    def clean_alerts(self):
        if len(self.alerts) >= 100:
            new_alerts = []
            for i in range(len(self.alerts)):
                try:
                    new_alerts.append(self.alerts[i+1])
                except:
                    pass
            self.alerts.clear()
            self.alerts += new_alerts

    def update_sensors(self, key:str):
        if key not in self.sensors.keys():
            self.sensors.update({key:{}})
            
            with open('../etc/sensors.json', 'w') as cf:
                cf.write(json.dumps(self.sensors))

    def stop(self):
        self.updates.clear()

    def append_room(self, room:dict):
        name = room['room']

        remove_keys = []
        for key in list(self.rooms.keys()):
            if room['sensor'] == self.rooms[key]['sensor']:
                remove_keys += [key]
        
        for key in remove_keys:
            del self.rooms[key]

        self.rooms.update({name:{
            'sensor':room['sensor'], 
            'roomHeight': room['roomHeight'],
            'roomWidth': room['roomWidth'],
            'roomLength': room['roomLength'],
            'nrOfWindows': room['nrOfWindows'],
            'tDay': room['tDay'],
            'tNight': room['tNight'],
            'tBound': room['tBound'],
            'ventilation': room['ventilation']}})

        self.write_rooms()

    def write_rooms(self):
        with open('../etc/rooms.json', 'w') as cf:
            cf.write(json.dumps(self.rooms))

    def update_setpoint(self, payload:dict):
        sensor = payload['sensor']
        for key in list(self.rooms.keys()):
            if sensor == self.rooms[key]['sensor']:
                
                day = payload['time']
                if day == "day":
                    self.rooms[key]['tDay'] = int(payload['setpoint'])
                elif day == "night":
                    self.rooms[key]['tNight'] = int(payload['setpoint'])
