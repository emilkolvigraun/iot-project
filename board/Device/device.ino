#include <WiFi.h>
#include <PubSubClient.h>
#include "src/sensor_manager.h"
#include <ArduinoJson.h>
#include "SD.h"
#include <WiFiUdp.h>
#include <NTPClient.h> 

#define NTP_OFFSET   0      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"

#define SD_CLK  14
#define SD_DO   2
#define SD_DI   15
#define SD_CS   13 

File configFile;
StaticJsonDocument<600> configJsonFile;

const char* ssid = "Luke SkyRouter";
const char* pass = "NT7TVT4WS3HAFX";

const char* mqtt_server_ip = "192.168.1.133"; 
int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeNTPClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

String MAC_ADDRESS = "UNSET";

bool SD_CARD_AVAILABLE = false;

// init loaded config
bool loadedConfig = false;

// ROOM definitions
String roomName;
int roomHeight;
int roomWidth;
int roomLength;
int nrOfWindows;
int temperatureSetpointDay;
int temperatureSetpointNight;
bool ventilation;

/* initialize sd card connection */
bool initSDCard(){

  // Initialize SD card
  SPI.begin(SD_CLK, SD_DO, SD_DI, SD_CS);
  if(!SD.begin(SD_CS)){
    Serial.println("No SD card available...");
    return false;
  }

  uint8_t cardType = SD.cardType();
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }

  configFile.close();
  return true;
}

void setup(){
  Serial.begin(9600);

  Serial.println("");
  
  if (initSDCard()){

    configFile = SD.open("/config.txt");

    if (configFile) {

    char content[configFile.size()];
    int i = 0;
    while(configFile.available()){
      int currentByte = configFile.read();
      char character = (char) currentByte;
      content[i] = character;
      i += 1;

      if (currentByte == -1){
        break;
      }
    }

    if (i > 10){
      encodeToJson(content);
    }

    configFile.close();
    } 

    SD_CARD_AVAILABLE = true;
  }

  // Connect to WiFi
  initialize_wifi();

  timeNTPClient.begin();

  //initialize sensors
  initialize_sensors();

  // Setup MQTT configurations
  client.setServer(mqtt_server_ip, port);
  client.setCallback(callback);

  if (!loadedConfig) {
    if (!client.connected()) {
      reconnect();
    }

    client.publish("sensor/registration", (MAC_ADDRESS+"/temperature").c_str());
  }
  
}

void writeToConfigFile(char* payload){

  if (SD.exists("/config.txt")) {
    SD.remove("/config.txt");
  } else {
    Serial.println("/config.txt doesn't exist.");
  }

  configFile = SD.open("/config.txt", FILE_WRITE);
  if (configFile){
    configFile.println(payload);
    configFile.close();
  } else {
    Serial.println("error opening /config.txt");
  }
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
  if (SD_CARD_AVAILABLE){
    writeToConfigFile(payload);
  }
  encodeToJson(payload);
}

void encodeToJson(char* payload){

// Test if parsing succeeds.
  if (deserializeJson(configJsonFile, payload)) {
    return;
  }

  const char* room = configJsonFile["room"];
  unsigned int nrWin = configJsonFile["nrOfWindows"];

  unsigned int rWidth = configJsonFile["roomWidth"];
  unsigned int rHeight = configJsonFile["roomHeight"];
  unsigned int rLength = configJsonFile["roomLength"];

  unsigned int tDay = configJsonFile["tDay"];
  unsigned int tNight = configJsonFile["tNight"];

  bool vent = configJsonFile["ventilation"];

  roomName = room;
  nrOfWindows  = nrWin;

  roomHeight = rHeight;
  roomWidth  = rWidth;
  roomLength = rLength;

  temperatureSetpointDay = tDay;
  temperatureSetpointNight = tNight;
  ventilation = vent;

  Serial.print("ventilation: ");
  Serial.println(ventilation);

  loadedConfig = true;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

    // Attempt to connect
    if (client.connect(MAC_ADDRESS.c_str())) {
      Serial.println(("Subscribed to: "+MAC_ADDRESS+"/room/config").c_str());
      client.subscribe((MAC_ADDRESS+"/room/config").c_str());
    } else {
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

int getSetpoint(bool daytime){
  if (daytime){
    return temperatureSetpointDay;
  } else {
    return temperatureSetpointNight;
  }
}

bool isDay(int currentHour){
  if (currentHour > 6 && currentHour < 21){
    return true;
  }
  return false;
}

int calculateVentilationWattage(int currentHour, float currentTemperature){
  bool daytime = isDay(currentHour);
  int setpoint = getSetpoint(daytime);
  float delta;
  if (currentTemperature > setpoint) {
    delta = currentTemperature - setpoint;
  } else {
    delta = setpoint - currentTemperature;
  }
  return (delta * 8.4) * 100; // scaled to something that seems fair
}

float calculateAirMass(){
  return 1;
}

float calculateVentilationEffect(){
  return 1;
}

void loop(){
  timeNTPClient.update();

  if (loadedConfig) {

    if (!client.connected()) {
      Serial.println("Reconnecting...");
      reconnect();
    }

    client.loop();

    float temperature     = get_temperature();
    String lightSensor    = (String) get_ambientLight();
    String humiditySensor = (String) get_humidity();

    unsigned long epochTime = timeNTPClient.getEpochTime();

    int hour = timeNTPClient.getHours()+2;
    int ventilationWattage =  calculateVentilationWattage(hour, temperature);

  }
} 