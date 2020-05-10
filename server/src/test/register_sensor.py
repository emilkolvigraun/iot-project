from mqtt_client import Receiver, MQTTClient

receiver = Receiver()

client = MQTTClient(receiver)

client.start_loop()
client.publish('sensor/registration', 'TEST_SENSOR')
client.subscribe('TEST_SENSOR/room/config')

try:
    while True: 
        client.wait(1)
except KeyboardInterrupt:
    client.stop_loop()
    client.disconnect()

