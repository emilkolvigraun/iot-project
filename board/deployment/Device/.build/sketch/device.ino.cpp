#include <Arduino.h>
#line 1 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
#include <WiFi.h>
#include <PubSubClient.h>
#include "src/sensor_manager.h"
#include <ArduinoJson.h>
#include "SD.h"
#include <WiFiUdp.h>
#include <NTPClient.h> 

// system time function   
#define seconds()(millis())
 
// Offsets for the timeserver 
#define     NTP_OFFSET   0      // In seconds
#define     NTP_INTERVAL 60 * 1000    // In miliseconds
#define     NTP_ADDRESS  "europe.pool.ntp.org"
WiFiUDP     ntpUDP;
NTPClient timeNTPClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// SD card pins 
#define SD_CLK  14 
#define SD_DO   2
#define SD_DI   15
#define SD_CS   13

// json file variables
File                    file;
StaticJsonDocument<600> config;
StaticJsonDocument<600>  setpoint;

// ROOM configuration variables
String          roomName;
int             roomHeight;
int             roomWidth;
int             roomLength;
int             nrOfWindows;
int             temperatureSetpointDay; 
int             temperatureSetpointNight;
bool            ventilation;
float           temperatureBound;

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
 
// epoch time
unsigned long   currentTime         = 0; 
  
// System time variables  
const double    UPDATE_TIMER        = 100; // milliseconds

double          lastUpdate          = 0.0; 
double          systemTime          = 0.0; 
double          timeTaken           = 0.0; 
int             currentHour         = 0;
const float     TEMP_CHANGE_BOUND   = 0.1;  // celcius
const float     HUMI_CHANGE_BOUND   = 1;    // % 
const float     LUX_CHANGE_BOUND    = 2;    // lux
  
// Environment variables
bool            daytime             = false; 
bool            SD_CARD_AVAILABLE   = false; 
bool            loadedConfig        = false; 
bool            tempRunning         = false;
bool            humRunning          = false;
bool            luxRunning          = false;

// used for running average
int             tempOldestIndex     = 0;  
int             humiOldestIndex     = 0;  
int             luxOldestIndex      = 0;  
const int       TEM_AVG_LENGTH      = 125; 
const int       HUM_AVG_LENGTH      = 100; 
const int       LUX_AVG_LENGTH      = 75; 
 
float           temperatureAvg[TEM_AVG_LENGTH]; 
float           humidityAvg[HUM_AVG_LENGTH]; 
float           lightAvg[LUX_AVG_LENGTH]; 

float           lastReportedTem     = 0;
float           lastReportedHum     = 0;
float           lastReportedVen     = 0; 
float           lastReportedLux     = 0; 
 
double          venWattage          = 0;
double          venCelcius          = 0;
double          venDiff             = 0;

#line 97 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool initSDCard();
#line 107 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void writeToConfigFile(char* payload);
#line 119 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void initWifi();
#line 127 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void onMessageReceived(char* topic, byte* message, unsigned int length);
#line 145 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void updateRelevantSetpoint(char* data);
#line 158 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void encodeToJson(char* payload);
#line 183 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool loadConfigurationFile();
#line 206 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void reconnect();
#line 218 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
int getSetpoint();
#line 226 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool isDay(int currentHour);
#line 233 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double calculateVentilationWattage(float currentTemperature);
#line 241 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double calculateAirMass(float temperature);
#line 249 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double calculateVentilationTDiff(double joules, double temperature);
#line 253 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double validateVentilationImpactDirection(double effect, double temperature);
#line 260 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool isConnectedToMqtt();
#line 267 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
float sumMean(float readings[], int length);
#line 275 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool valueChangedEnough(float value, float previousValue, float limit);
#line 282 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void transmitTemperatureIfDataChanged(float temperature);
#line 309 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void transmitHumidityIfDataChanged(float humidity);
#line 317 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void transmitLuxIfDataChanged(float lux);
#line 324 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void setup();
#line 349 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void loop();
#line 97 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool initSDCard(){
    SPI.begin(SD_CLK, SD_DO, SD_DI, SD_CS);
    if(!SD.begin(SD_CS)){
        return false;
    }

    file.close(); 
    return true;
}

void writeToConfigFile(char* payload){
    if (SD.exists("/config.txt")) {
        SD.remove("/config.txt");
    } 

    file = SD.open("/config.txt", FILE_WRITE);
    if (file){ 
        file.println(payload); 
        file.close();
    } 
}

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
    if (((String) topic) == (MAC_ADDRESS+"/room/config")){
        if (SD_CARD_AVAILABLE){
        writeToConfigFile(payload); 
        }
        encodeToJson(payload);
    } else if (((String) topic) == (MAC_ADDRESS+"/setpoint")){
        updateRelevantSetpoint(payload);
    }
    Serial.println("received:");
    Serial.println(payload); 
}
 
void updateRelevantSetpoint(char* data){
    if (deserializeJson(setpoint, data)) {
        return;
    }
 
    int sp = setpoint["setpoint"]; 
    if (currentHour > 6 && currentHour < 21){
        temperatureSetpointDay = sp;  
    } else { 
        temperatureSetpointNight = sp;
    }
}

void encodeToJson(char* payload){
    if (deserializeJson(config, payload)) {
        return;
    }
    const char* room = config["room"];
    unsigned int nrWin = config["nrOfWindows"];
    unsigned int rWidth = config["roomWidth"];
    unsigned int rHeight = config["roomHeight"];
    unsigned int rLength = config["roomLength"];
    unsigned int tDay = config["tDay"];
    unsigned int tNight = config["tNight"];
    bool vent = config["ventilation"];
    float bound = config["tBound"];
    roomName = room;
    nrOfWindows  = nrWin;
    roomHeight = rHeight; 
    roomWidth  = rWidth;
    roomLength = rLength;
    temperatureSetpointDay = tDay;
    temperatureSetpointNight = tNight; 
    temperatureBound = bound;
    ventilation = vent;
    loadedConfig = true; 
}

bool loadConfigurationFile(){
    file = SD.open("/config.txt");
    if(file){
        char content[file.size()];
        int i = 0;
        while(file.available()){
            int currentByte = file.read();
            char character = (char) currentByte;
            content[i] = character;
            i += 1; 

            if (currentByte == -1){
                break;
            }
        } 

        if (i > 150){
            encodeToJson(content); 
        } 
        file.close();
    }
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect(MAC_ADDRESS.c_str())) {
            client.subscribe((MAC_ADDRESS+"/room/config").c_str());
            client.subscribe((MAC_ADDRESS+"/setpoint").c_str());
            Serial.println("subscribed");
        } else {
            delay(1000);
        }
    }
}

int getSetpoint(){
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
 
double calculateVentilationWattage(float currentTemperature){
    if (currentTemperature > getSetpoint()) { 
        return ((currentTemperature - getSetpoint()) * 8.4) * 100; 
    } else {
        return ((getSetpoint() - currentTemperature) * 8.4) * 100;
    }
}
 
double calculateAirMass(float temperature){
    double mass =  (101.325*1000) / (287.058 * (temperature + 273.15)) * (roomHeight * roomLength * roomWidth);
    if (mass < 0){ 
        mass = mass * -1; 
    }
    return mass;  
}
  
double calculateVentilationTDiff(double joules, double temperature){
    return (joules / calculateAirMass(temperature)) / 1012;
} 

double validateVentilationImpactDirection(double effect, double temperature){
    if (temperature > getSetpoint()){
        return -effect;
    } 
    return effect;
}

bool isConnectedToMqtt(){
    if (!client.connected()) {
        reconnect();
    }  
    return true;
} 
 
float sumMean(float readings[], int length){ 
    float summation = 0;   
    for (int i = 0; i < length; i++){
        summation += readings[i];
    }
    return summation/length;
}

bool valueChangedEnough(float value, float previousValue, float limit){
    if (value > previousValue+limit || value < previousValue-limit){ 
        return true;
    } 
    return false; 
}

void transmitTemperatureIfDataChanged(float temperature){

    venWattage = 0;
    if (ventilation){
        temperature += venCelcius;  
        venWattage = calculateVentilationWattage(temperature);  
        venDiff = calculateVentilationTDiff(venWattage * (UPDATE_TIMER/1000), temperature);
        venDiff = validateVentilationImpactDirection(venDiff, temperature);
        venCelcius += venDiff;
        temperature += venDiff; 
        Serial.println(lastReportedTem+TEMP_CHANGE_BOUND, 1);
    } 
   
    currentTime = timeNTPClient.getEpochTime(); 
    if (valueChangedEnough(temperature, lastReportedTem, TEMP_CHANGE_BOUND)){
        client.publish((MAC_ADDRESS+"/temperature").c_str(), (((String) temperature)+","+roomName+","+((String)currentTime)+","+((String)tempOldestIndex)).c_str());
        lastReportedTem = temperature;
    }  
    if (valueChangedEnough(venWattage, lastReportedVen, 100)){
        client.publish((MAC_ADDRESS+"/ventilation").c_str(), (((String) venWattage)+","+roomName+","+((String)currentTime)).c_str());
        lastReportedVen = venWattage;  
    }   
    if (valueChangedEnough(temperature, getSetpoint(), temperatureBound)){
        client.publish("sensor/alert", ("room="+roomName+", temperature="+(String) temperature).c_str());
    }
}

void transmitHumidityIfDataChanged(float humidity){ 
    if (valueChangedEnough(humidity, lastReportedHum, HUMI_CHANGE_BOUND)){
        client.publish((MAC_ADDRESS+"/humidity").c_str(), (((String) humidity)+","+roomName+","+((String)currentTime)+","+((String)humiOldestIndex)).c_str());
        lastReportedHum = humidity;
    }
 
}

void transmitLuxIfDataChanged(float lux){ 
    if (valueChangedEnough(lux, lastReportedLux, LUX_CHANGE_BOUND)){
        client.publish((MAC_ADDRESS+"/lux").c_str(), (((String) lux)+","+roomName+","+((String)currentTime)+","+((String)luxOldestIndex)).c_str());
        lastReportedLux = lux; 
    }  
}

void setup() {
    Serial.begin(9600);  
    delay(1);
 
    if (initSDCard()){ 
        loadConfigurationFile();
        SD_CARD_AVAILABLE = true; 
    }

    initWifi();
    timeNTPClient.begin();
    initSensors();

    client.setServer(mqtt_server_ip, port);
    client.setCallback(onMessageReceived);

    if (isConnectedToMqtt()){  
        client.publish("sensor/registration", (MAC_ADDRESS+"/temperature").c_str());
        client.publish("sensor/registration", (MAC_ADDRESS+"/humidity").c_str());
        client.publish("sensor/registration", (MAC_ADDRESS+"/lux").c_str());
        client.publish("sensor/registration", (MAC_ADDRESS+"/ventilation").c_str());
    }
 
} 

void loop(){ 

    if (!client.connected()) {
        reconnect();
    }  
    client.loop();
    
    if (loadedConfig){
        systemTime = seconds();
        if (systemTime - lastUpdate >= UPDATE_TIMER){
            timeNTPClient.update(); 
            currentHour = timeNTPClient.getHours()+2;  
            daytime = isDay(currentHour);

            temperatureAvg[tempOldestIndex]  = get_temperature();
            humidityAvg[humiOldestIndex]     = get_humidity();
            lightAvg[luxOldestIndex]         = get_ambientLight(); 
            
            if (tempRunning){
                transmitTemperatureIfDataChanged(sumMean(temperatureAvg, TEM_AVG_LENGTH));
            }
            if (humRunning){
                transmitHumidityIfDataChanged(sumMean(humidityAvg, HUM_AVG_LENGTH));
            }
            if (luxRunning){
                transmitLuxIfDataChanged(sumMean(lightAvg, LUX_AVG_LENGTH)); 
            }
    
            tempOldestIndex += 1;
            humiOldestIndex += 1;
            luxOldestIndex  += 1;

            if (tempOldestIndex >= TEM_AVG_LENGTH-1){  
                tempRunning     = true;
                tempOldestIndex = 0;
            } 
            if (humiOldestIndex >= HUM_AVG_LENGTH-1){  
                humRunning      = true;
                humiOldestIndex = 0;
            }
            if (luxOldestIndex >= LUX_AVG_LENGTH-1){  
                luxRunning      = true;
                luxOldestIndex  = 0;
            }

            timeTaken = seconds(); 
            lastUpdate = timeTaken - (systemTime - timeTaken); 
        }
    }
}

