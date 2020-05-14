
from mqtt_client import Receiver, MQTTClient
from database import Database
import json

class StorageEngine(Receiver):

    db: Database = Database() 
    return_topics: dict = {}
    subs: list = []

    def __init__(self):
        try:
            with open('subs.json', 'r') as f:
                self.subs = json.loads(''.join(f.readlines()))['subscriptions']
        except: 
            pass

    def get_subs(self):
        return self.subs

    def update_subscriptions(self, sub):
        if sub not in self.subs:
            self.subs += [sub]
            with open('subs.json', 'w') as f:
                f.write(json.dumps({'subscriptions':self.subs}))

    def on_message(self, client, userdata, message):
        msg = str(message.payload.decode("utf-8"))

        if message.topic == 'archiver/subscribe':
            client.subscribe(msg)
            self.update_subscriptions(msg)

        elif message.topic == 'archiver/get':
            debunked_message = msg.split(',')
            table = debunked_message[0].replace(':','-')
            data = self.db.get_where(debunked_message[1], debunked_message[2], table)
            client.publish(debunked_message[3], json.dumps(data))

            if table not in self.return_topics.keys():
                self.return_topics.update({table:[]})

            self.return_topics[table].append(debunked_message[3])

        elif message.topic == 'archiver/stop':
            debunked_message = msg.split(',')
            table = debunked_message[0].replace(':', '-')
            print('message ->', debunked_message)
            print('before ->', self.return_topics[table])
            if table in self.return_topics.keys():
                try:
                    index = self.return_topics[table].index(debunked_message[1])
                    del self.return_topics[table][index]
                except Exception as e:
                    print(e)
            print('after ->', self.return_topics[table])
        else:
            debunked_message = msg.split(',')
            debunked_topic   = message.topic.split('/')
            table = debunked_message[1] + '_' + debunked_topic[0]
            table = table.replace(':', '-')

            try:
                if not self.db.table_exists(table):
                    self.db.create(table)
                else: 
                    self.db.insert(table, debunked_message[2], debunked_topic[1], debunked_message[0])
            except Exception as e:
                print(e)

            if table in self.return_topics.keys():
                for topic in self.return_topics[table]:
                    client.publish(topic, json.dumps([(float(debunked_message[0]), debunked_topic[1])]))
            
class Archiver:

    client: MQTTClient = MQTTClient(StorageEngine, 'Archiver')

    def __init__(self):
        self.client.start_loop()
        self.client.subscribe('archiver/subscribe')
        self.client.subscribe('archiver/get')
        self.client.subscribe('archiver/stop')

    def run(self):

        try:
            while True:
                self.client.wait(2)
        except:
            self.client.stop_loop()
            self.client.disconnect()
            print('quitting')
