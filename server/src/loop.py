
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
            self.com.register_sensor(msg)


class Loop(Thread):

    com: common.Common = common.Common()
    mqtt: MQTTClient = MQTTClient(MessageReceiver(com), 'HTTPLooper')

    def __init__(self, config, loop_time:float=1.0/60):
        self.mqtt.start_loop()
        for topic in config['topics']['subscribe']:
            self.mqtt.subscribe(topic)
        super(Loop, self).__init__()

    def run(self):

        jobs = len(self.com.updates)

        while jobs > 0:
            for i in range(1, jobs):
                
                if self.com.updates[i][0] == common.PUBLISH:
                    self.mqtt.publish(self.com.updates[i][1], self.com.updates[i][2])
                elif self.com.updates[i][0] == common.SUBSCRIBE:
                    self.mqtt.subscribe(self.com.updates[i][1])

            jobs = len(self.com.updates)

        self.mqtt.stop_loop()

    def stop(self):
        self.com.updates.clear()

