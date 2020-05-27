# 1 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino"
# 2 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino" 2
# 3 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino" 2
# 4 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino" 2
# 5 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino" 2
# 6 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino" 2
# 7 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino" 2
# 8 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\system latency\\Device\\device.ino" 2


// wifi password and name definitons
const char* ssid = "Luke SkyRouter";
const char* pass = "NT7TVT4WS3HAFX";
WiFiClient espClient;

// device MAC address used for identification
String MAC_ADDRESS = "UNSET";

// MQTT server details
const char* mqtt_server_ip = "192.168.1.133";
int port = 1883;
PubSubClient client(espClient);

int counter = 0;

void initWifi(){
    WiFi.begin(ssid, pass);
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
    }
    MAC_ADDRESS = WiFi.macAddress();
}

void onMessageReceived(char* topic, byte* message, unsigned int length) {
   client.publish("latency/response", ((String)counter).c_str());
   counter += 1;
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
