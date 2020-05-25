from mqtt_client import Receiver, MQTTClient
import json, time, random, uuid

class OnReceive(Receiver):

    def on_message(self, client, userdata, message):
        msg = str(message.payload.decode("utf-8"))


# get_room_data()
receiver = OnReceive()
client = MQTTClient(receiver)
   
client.start_loop()
client.publish("30:AE:A4:DC:9A:38/setpoint", json.dumps({"setpoint":30}))
client.wait(2)
client.stop_loop()