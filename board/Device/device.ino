#include <WiFi.h>
#include <PubSubClient.h>
#include "src/sensor_manager.h"


const char* ssid = "Luke SkyRouter";
const char* pass = "NT7TVT4WS3HAFX";

const char* mqtt_server_ip = "192.168.1.133";
int port = 1883;

unsigned long time;

WiFiClient espClient;
PubSubClient client(espClient);


void setup(){
  Serial.begin(9600);

  setup_wifi();
  client.setServer(mqtt_server_ip, port);
  client.setCallback(callback);
  
}

void setup_wifi(){
  Serial.println("");
  Serial.println("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println(message);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32")) {
      Serial.println("connected");
      client.subscribe("time/sync/current")

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String tempSensor = (String) get_temperature();
  String lightSensor = (String) get_ambientLight();
  String humiditySensor = (String) get_humidity();

      // Publish
    client.publish("sensor/registration", "fuck");
    client.publish("sensor/registration", "dig");
    client.publish("sensor/registration", "emil");
    delay(500);
  
}