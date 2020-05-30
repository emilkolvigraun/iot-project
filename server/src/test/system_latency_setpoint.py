# http("http://192.168.1.133:8000/configuration/room/setpoint/update", "POST", JSON.stringify({"sensor":currentSensorSelection, "setpoint":currentSetpointValue, "time":time}))

import requests, random, time, json
from mqtt_client import Receiver, MQTTClient

class OnReceive(Receiver):

    def on_message(self, client, userdata, message):
        msg = str(time.time())+","+str(message.payload.decode("utf-8")) 
        print(msg) 

receiver = OnReceive() 
client = MQTTClient(receiver, 'latency_tester')
client.subscribe('latency/response') 

try: 
    client.start_loop()
    
    timer = time.time()  
    update = 1/1000
    sender = 0 
    while True:  
            t1 = time.time()     
            t0 = t1 - timer  
            if t0 >= update:
                print(str(time.time())+",send,"+str(sender))
                requests.post('http://192.168.1.133:8000/configuration/room/setpoint/update', data=json.dumps({"sensor":'30:AE:A4:DF:9A:6C', "counter":str(sender), "time":'day'}))
                sender += 1
                t2 = time.time() 
                timer = t2-(t1-t2)
except KeyboardInterrupt:
    client.stop_loop()
    client.disconnect() 
    exit('exited')