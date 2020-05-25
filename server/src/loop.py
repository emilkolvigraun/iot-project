
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
        if message.topic == 'sensor/alert':
            self.com.register_alert(msg)


class Loop(Thread):

    com: common.Common = common.Common()
    mqtt: MQTTClient = MQTTClient(MessageReceiver(com), 'HTTPLooper')

    def __init__(self, config):
        self.mqtt.start_loop()
        for topic in config['topics']['subscribe']:
            self.mqtt.subscribe(topic)
        super(Loop, self).__init__()

    def run(self):

        jobs = self.com.number_of_updates()

        while jobs > 0:
            for task in self.com.tasks():

                if task[0] == common.PUBLISH:
                    self.mqtt.publish(task[1], task[2])
                elif task[0] == common.SUBSCRIBE:
                    self.mqtt.subscribe(task[1])

            jobs = self.com.number_of_updates()

        self.mqtt.stop_loop()

    def stop(self):
        self.com.stop()

