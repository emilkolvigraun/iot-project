from mqtt_client import Receiver, MQTTClient

receiver = Receiver()

client = MQTTClient(receiver)

client.start_loop()
client.publish('sensor/registration', 'TEST_SENSOR')

client.wait(2)
client.stop_loop()

client.disconnect()

