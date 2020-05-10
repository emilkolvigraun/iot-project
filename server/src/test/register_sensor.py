from mqtt_client import Receiver, MQTTClient

receiver = Receiver()

client = MQTTClient(receiver)

client.start_loop()
client.publish('sensor/registration', 'TEST_SENSOR')
client.subscribe('TEST_SENSOR/room/config')

try:
    while True: 
        client.publish('TEST_SENSOR/temperature', '20')
        client.wait(2)
except KeyboardInterrupt:
    client.stop_loop()
    client.disconnect()

