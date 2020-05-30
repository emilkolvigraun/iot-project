#include <Arduino.h>
#line 1 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
#include <WiFi.h>
#include <PubSubClient.h>
#include "src/sensor_manager.h"
#include <ArduinoJson.h>

 
// wifi password and name definitons
const char*     ssid                = "Luke SkyRouter";
const char*     pass                = "NT7TVT4WS3HAFX";
WiFiClient      espClient;

// device MAC address used for identification
String          MAC_ADDRESS         = "UNSET";

// MQTT server details
const char*     mqtt_server_ip      = "192.168.1.133"; 
int             port                = 1883;
PubSubClient    client(espClient);
 
StaticJsonDocument<300>  jsonFile;

#line 22 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
void initWifi();
#line 30 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
void onMessageReceived(char* topic, byte* message, unsigned int length);
#line 45 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
void reconnect();
#line 55 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
void setup();
#line 63 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
void loop();
#line 22 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
void initWifi(){
    WiFi.begin(ssid, pass);
    while(WiFi.status() != WL_CONNECTED){ 
        delay(500);
    }
    MAC_ADDRESS = WiFi.macAddress();
}   
  
void onMessageReceived(char* topic, byte* message, unsigned int length) {
    char payload[length];  
    for (int i=0;i<length;i++) 
    {
        payload[i] = (char)message[i];
    }  
    if (deserializeJson(jsonFile, payload)) {
        client.publish("latency/response", "received,NaN"); 
        return;
    }     
    const char* receivedCounter = jsonFile["counter"]; 
    String received = (String) receivedCounter; 
    client.publish("latency/response", ("received,"+received).c_str());
} 
 
void reconnect() {
    while (!client.connected()) {
        if (client.connect(MAC_ADDRESS.c_str())) {
            client.subscribe((MAC_ADDRESS+"/setpoint").c_str());
        } else {
            delay(1000);
        }
    } 
}

void setup() {
    Serial.begin(9600);  
    delay(1); 
    initWifi();
    client.setServer(mqtt_server_ip, port); 
    client.setCallback(onMessageReceived); 
} 

void loop(){ 

    if (!client.connected()) {
        reconnect();
    }  
    client.loop();
}

