# http("http://192.168.1.133:8000/configuration/room/setpoint/update", "POST", JSON.stringify({"sensor":currentSensorSelection, "setpoint":currentSetpointValue, "time":time}))

import requests, random, time, json
from mqtt_client import Receiver, MQTTClient

class OnReceive(Receiver):

    def on_message(self, client, userdata, message):
        msg = str(message.payload.decode("utf-8"))
 
        with open('latency_log.txt', mode='a') as f:
            f.write(str(time.time())+","+msg+"\n")

receiver = OnReceive() 
client = MQTTClient(receiver, 'latency_tester')
client.subscribe('latency/response')
client.start_loop()

try: 
    print('listening...')
    timer = time.time()  
    hz = 0
    counter = 0 
    sender = 0
    update = 1/hz if hz > 0 else 1 

    while True:
            t1 = time.time() 
            t0 = t1 - timer
            if t0 >= update:
                requests.post('http://192.168.1.133:8000/configuration/room/setpoint/update', data=json.dumps({"sensor":'30:AE:A4:DC:9A:38', "setpoint":random.randint(20,30), "time":'day'}))
                with open('latency_log_send.txt', mode='a') as f:
                    f.write(str(time.time())+","+str(sender)+"\n")
                counter += 1
                sender += 1
                if counter > 1000:
                    hz += 10
                    counter = 0

                t2 = time.time() 
                timer = t2-(t1-t2)
except KeyboardInterrupt:
    client.stop_loop()
    client.disconnect()
    exit('exited')