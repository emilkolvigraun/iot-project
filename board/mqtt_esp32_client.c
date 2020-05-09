#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Supreben";
const char* pass = "Morten39";

const char* mqtt_server_ip = "192.168.0.45";


WiFiClient espClient;
PubSubClient client(espClient);


void setup(){

  Serial.begin(9600);

  setup_wifi();
  client.setServer(mqtt_server_ip, 1883);
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
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("myclient")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
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
  
}
