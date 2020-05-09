
from threading import Thread
from mqtt_client import MQTTClient, Receiver
import common

class MessageReceiver(Receiver):

    def __init__(self, com:common.Common):
        self.com = com

    def on_message(self, client, userdata, message):
        
        # first decode message
        msg = str(message.payload.decode("utf-8"))

        # act based on topic
        if message.topic == 'sensor/registration':
            if msg not in self.com.sensors.keys(): self.com.sensors.update({msg:{}})


class Loop(Thread):

    com: common.Common = common.Common()
    mqtt: MQTTClient = MQTTClient(MessageReceiver(com), 'HTTPLooper')

    def __init__(self, config, loop_time:float=1.0/60):
        self.mqtt.start_loop()
        for topic in config['topics']['subscribe']:
            self.mqtt.subscribe(topic)
        super(Loop, self).__init__()

    def run(self):
        self.mqtt.stop_loop()

