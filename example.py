from mqtt_client import Receiver, MQTTClient


receiver = Receiver()

client = MQTTClient(receiver)

client.start_loop()

client.subscribe('test/topic')
client.publish('test/topic', 'DATA')

client.wait(2)
client.stop_loop()

client.disconnect()

