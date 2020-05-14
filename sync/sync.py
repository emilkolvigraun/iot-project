from mqtt_client import MQTTClient
import time

client = MQTTClient()

client.start_loop()

while True:
    client.publish('time/sync/current', str(time.time()))
    time.sleep(1)