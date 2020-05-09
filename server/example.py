from mqtt_client import Receiver, MQTTClient


receiver = Receiver()

client = MQTTClient(receiver)

client.start_loop()

client.subscribe('test/topic')

for i in range(10):
    client.publish('test/topic', 'DATA'+str(i))

client.wait(2)
client.stop_loop()

client.disconnect()

