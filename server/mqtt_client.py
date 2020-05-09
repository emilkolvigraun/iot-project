# python 3.7.4

# required libraries:
# paho-mqtt
# time

import paho.mqtt.client as mqtt
from time import sleep
from log import Log
log = Log().log

# Authours:
# Emil Stubbe Kolvig-Raun, emstu15@student.sdu.dk
# Morten Abrahamsen Olesen, moole15@student.sdu.dk

# Receiver interface, passed to an MQTT client
class Receiver:

    # method must be overwritten
    def on_message(self, client, userdata, message):
        # default method logs received message
        msg = 'received message: %s, from topic: %s'%(str(message.payload.decode("utf-8")), message.topic)
        log('DEFAULT_RECEIVER', msg) 

# mosquitto client implementation
class MQTTClient:

    # TODO: read the settings from a configuration file
    broker_address: str = '127.0.0.1'
    broker_port: int = 1883

    # initialize mqtt client details
    client_id: str = 'debug'
    client: mqtt.Client = mqtt.Client(client_id)

    def __init__(self, receiver:Receiver, client_id:str=None):

        if client_id is not None:
            self.client_id = client_id
            self.client = mqtt.Client(self.client_id)

        # the receiver is now in global class scope
        self.receiver = receiver

        # connects to the broker on init
        self.client.connect(self.broker_address, port=self.broker_port)

        # configures on message binding
        self.client.on_message = self.receiver.on_message

    def start_loop(self):
        # this will create a new background thread
        # for the client to process its tasks
        self.client.loop_start()

    def publish(self, topic:str, message:str):
        # start_loop must be called prior to subscribing
        self.client.publish(topic, message)

    def subscribe(self, topic:str):
        self.client.subscribe(topic)

    def stop_loop(self):
        # this will join the background thread
        # and clear memory
        self.client.loop_stop()

    def wait(self, seconds:int):
        sleep(seconds)

    def reconnect(self):
        # if the client looses connection
        # or it becomes apparent, that the connection must be reestablished
        self.disconnect()
        self.client.connect(self.broker_address, port=self.broker_port)

        # reconfigures the on message binding
        self.client.on_message = self.receiver.on_message

    def disconnect(self):
        # tries to disconnect the client
        try:
            self.client.disconnect()
        except:
            # throws an error, if there is no connection to begin with
            pass




