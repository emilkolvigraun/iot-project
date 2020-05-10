
from mqtt_client import Receiver, MQTTClient
from database import Database

class StorageEngine(Receiver):

    db: Database = Database() 

    def on_message(self, client, userdata, message):
        # default method logs received message
        msg = str(message.payload.decode("utf-8"))

        if message.topic == 'archiver/subscribe':
            client.subscribe(msg)
        elif message.topic == 'archiver/get':
            print(msg)

class Archiver:

    client: MQTTClient = MQTTClient(StorageEngine(), 'Archiver')

    def __init__(self):
        self.client.start_loop()
        self.client.subscribe('archiver/subscribe')

    def run(self):

        try:
            while True:
                self.client.wait(2)
        except:
            self.client.stop_loop()
            self.client.disconnect()
            print('quitting')
