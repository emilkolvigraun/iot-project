from mqtt_client import Receiver, MQTTClient
import json, time, random, uuid

sensor_name = 'AS:45:d6:YH:7J' # get device mac address
room_name = None

def update_config(data):
    with open('config.json', 'w') as f:
        f.write(json.dumps(data))

def get_room_data():
    global room_name
    try:
        with open('config.json', 'r') as f:
            room_config = json.loads(''.join(f.readlines()))
            room_name = room_config['room']
    except:
        room_name = None
        pass

def get_lux():
    return random.random()*400
def get_temperature():
    return random.random()*10+10
def get_humidity():
    return random.random()*60

class OnReceive(Receiver):

    def on_message(self, client, userdata, message):
        global room_name

        msg = str(message.payload.decode("utf-8"))

        if message.topic == sensor_name+'/room/config':
            msg = json.loads(msg)
            room_name = msg['room']
            update_config(msg)
            print(msg)

# get_room_data()
receiver = OnReceive()
client = MQTTClient(receiver)
  
client.start_loop()
 
client.subscribe(sensor_name+'/room/config')

if room_name is None:
    client.publish('sensor/registration', sensor_name+'/temperature')
    client.publish('sensor/registration', sensor_name+'/humidity')
    client.publish('sensor/registration', sensor_name+'/lux')

try:
    while True: 
        if room_name != None:
            
            temperature = str(get_temperature())
            humidity = str(get_humidity())
            lux = str(get_lux())
            client.publish(sensor_name+'/temperature', temperature+','+room_name+','+str(time.time()))
            client.publish(sensor_name+'/humidity', humidity+','+room_name+','+str(time.time()))
            client.publish(sensor_name+'/lux', lux+','+room_name+','+str(time.time()))
        client.wait(2)
except KeyboardInterrupt:
    client.stop_loop()
    client.disconnect()

