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
StaticJsonDocument<600> setpointJsonFile;

const char* ssid = "Luke SkyRouter";
const char* pass = "NT7TVT4WS3HAFX";

const char* mqtt_server_ip = "192.168.1.133";
int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP ntpUDP;
NTPClient timeNTPClient(ntpUDP, "europe.pool.ntp.org", 0 /* In seconds*/, 60 * 1000 /* In miliseconds*/);

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

float UPDATE_TIME = 2; // seconds
unsigned long lastUpdate = 0;

float ventilationEffect = 0;

/* initialize sd card connection */
bool initSDCard(){

  // Initialize SD card
  SPI.begin(14, 2, 15, 13);
  if(!SD.begin(13)){
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

    if (i > 50){
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
  client.setCallback(onMessageReceived);

  if (!client.connected()) {
    Serial.println("Reconnecting...");
    reconnect();
  }

  client.publish("sensor/registration", (MAC_ADDRESS+"/temperature").c_str());
  client.publish("sensor/registration", (MAC_ADDRESS+"/humidity").c_str());
  client.publish("sensor/registration", (MAC_ADDRESS+"/lux").c_str());
  client.publish("sensor/registration", (MAC_ADDRESS+"/ventilation").c_str());
}

void writeToConfigFile(char* payload){

  if (SD.exists("/config.txt")) {
    SD.remove("/config.txt");
  } else {
    Serial.println("/config.txt doesn't exist.");
  }

  configFile = SD.open("/config.txt", "w");
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

void onMessageReceived(char* topic, byte* message, unsigned int length) {
  char payload[length];
  for (int i=0;i<length;i++)
  {
    payload[i] = (char)message[i];
  }

  if (topic == (MAC_ADDRESS+"/room/config").c_str()){
    Serial.println('room/config');
    if (SD_CARD_AVAILABLE){
      writeToConfigFile(payload);
    }
    encodeToJson(payload);
    Serial.println("Loaded new config.");
  } else if (topic == (MAC_ADDRESS+"/setpoint").c_str()){
    Serial.println("ADJUST CURRENT SETPOINT!");
    updateRelevantSetpoint(payload);
  }
}

void updateRelevantSetpoint(char* data){
  if (deserializeJson(setpointJsonFile, data)) {
    return;
  }

  int setpoint = setpointJsonFile["setpoint"];
  int hour = timeNTPClient.getHours()+2;
  if (hour > 6 && hour < 21){
    temperatureSetpointDay = setpoint;
  } else {
    temperatureSetpointNight = setpoint;
  }
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
  nrOfWindows = nrWin;

  roomHeight = rHeight;
  roomWidth = rWidth;
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
      client.subscribe((MAC_ADDRESS+"/room/config").c_str());
      client.subscribe((MAC_ADDRESS+"/setpoint").c_str());
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

float calculateVentilationWattage(int currentHour, float currentTemperature){
  bool daytime = isDay(currentHour);
  int setpoint = getSetpoint(daytime);
  Serial.print("Current setpoint: ");
  Serial.println(setpoint);
  float delta;
  if (currentTemperature > setpoint) {
    delta = currentTemperature - setpoint;
  } else {
    delta = setpoint - currentTemperature;
  }
  float wattage = (delta * 8.4) * 100;
  return wattage; // scaled to something that seems fair
}

float calculateAirMass(float currentTemperature){
  float pressure = (101.325*1000) / (287.058 * (currentTemperature * 273.15));
  float mass = pressure * (roomHeight * roomLength * roomWidth);

  if (mass < 0){
    mass = mass * -1;
  }

  return mass;
}

float calculateVentilationEffect(float joules, float currentTemperature){
  float numerator = joules / calculateAirMass(currentTemperature);
  return numerator / 1012;
}

float validateVentilationEffect(int currentHour, float effect, float temperature){
  bool daytime = isDay(currentHour);
  int setpoint = getSetpoint(daytime);
  if (temperature > setpoint){
    return -effect;
  }
  return effect;
}

void loop(){
  timeNTPClient.update();

  if (!client.connected()) {
    Serial.println("Reconnecting...");
    reconnect();
  }
  client.loop();

  unsigned long t0 = timeNTPClient.getEpochTime();
  double t1 = t0 - lastUpdate;
  if (t1 >= UPDATE_TIME){

    if (loadedConfig) {

      float temperature = get_temperature();
      String light = (String) get_ambientLight();
      String humidity = (String) get_humidity();

      unsigned long epochTime = timeNTPClient.getEpochTime();

      float ventilationWattage = 0;
      if (ventilation){
        temperature += ventilationEffect;
        int hour = timeNTPClient.getHours()+2;
        ventilationWattage = calculateVentilationWattage(hour, temperature);
        float effect = calculateVentilationEffect(ventilationWattage * UPDATE_TIME, temperature);
        effect = validateVentilationEffect(hour, effect, temperature);
        ventilationEffect += effect;
        temperature += effect;
      }

      unsigned long currentTime = timeNTPClient.getEpochTime();
    }
    unsigned long t2 = timeNTPClient.getEpochTime();
    lastUpdate = t2 - (t0 - t2);
  }
}