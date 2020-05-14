# 1 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino"
# 2 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino" 2
# 3 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino" 2
# 4 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino" 2
# 5 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino" 2
# 6 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino" 2
# 7 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino" 2
# 8 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\Device\\device.ino" 2

#define NTP_OFFSET 0 /* In seconds*/
#define NTP_INTERVAL 60 * 1000 /* In miliseconds*/
#define NTP_ADDRESS "europe.pool.ntp.org"

#define SD_CLK 14
#define SD_DO 2
#define SD_DI 15
#define SD_CS 13

File configFile;
StaticJsonDocument<600> configJsonFile;

const char* ssid = "Luke SkyRouter";
const char* pass = "NT7TVT4WS3HAFX";

const char* mqtt_server_ip = "192.168.1.133";
int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0 /* In seconds*/, 60 * 1000 /* In miliseconds*/);

String MAC_ADDRESS = "UNSET";

String roomName;

/* initialize sd card connection */
bool initSDCard(){

  // Initialize SD card
  SPI.begin(14, 2, 15, 13);
  if(!SD.begin(13)){
    Serial.println("Boot failed...");
    return false;
  }

  return true;
}

void setup(){
  Serial.begin(9600);

  // init sd card
  bool success = initSDCard();

  if (success){
    configFile = SD.open("config.json", "w");
    Serial.println("opened file");
  }

  // Connect to WiFi
  initialize_wifi();


  timeClient.begin();

  //initialize sensors
  initialize_sensors();

  // Setup MQTT configurations
  client.setServer(mqtt_server_ip, port);
  client.setCallback(callback);

  if (!client.connected()) {
    reconnect();
  }

  client.publish("sensor/registration", (MAC_ADDRESS+"/temperature").c_str());

}

void initialize_wifi(){

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  MAC_ADDRESS = WiFi.macAddress();
}

void callback(char* topic, byte* message, unsigned int length) {
  char payload[length];
  for (int i=0;i<length;i++)
  {
    payload[i] = (char)message[i];
  }
  DeserializationError error = deserializeJson(configJsonFile, payload);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(((reinterpret_cast<const __FlashStringHelper *>(("deserializeJson() failed: ")))));
    Serial.println(error.c_str());
    return;
  }

  const char* room = configJsonFile["room"];
  roomName = room;
  Serial.println(roomName);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

    // Attempt to connect
    if (client.connect(MAC_ADDRESS.c_str())) {
      client.subscribe((MAC_ADDRESS+"/room/config").c_str());
    } else {
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}


void loop(){
  timeClient.update();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String tempSensor = (String) get_temperature();
  String lightSensor = (String) get_ambientLight();
  String humiditySensor = (String) get_humidity();


  unsigned long epochTime = timeClient.getEpochTime();
}
