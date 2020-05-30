#include <Arduino.h>
#line 1 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
#include    <WiFi.h>
#include    <PubSubClient.h>
#include    "src/sensor_manager.h"
#include    <ArduinoJson.h>
#include    "SD.h"
#include    <WiFiUdp.h>
#include    <NTPClient.h> 

// system time function   
#define     milliseconds()(millis())
 
// Offsets for the timeserver 
#define     NTP_OFFSET   0      // In milliseconds
#define     NTP_INTERVAL 60 * 1000    // In milimilliseconds
#define     NTP_ADDRESS  "europe.pool.ntp.org"
WiFiUDP     ntpUDP;
NTPClient   timeNTPClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// SD card pins 
#define SD_CLK  14 
#define SD_DO   2
#define SD_DI   15
#define SD_CS   13

// json file variables
File                     file;
StaticJsonDocument<600>  config;
StaticJsonDocument<600>  setpoint;

// ROOM configuration variables
String          roomName; 
int             roomHeight;
int             roomWidth; 
int             roomLength;
int             temperatureSetpointDay; 
int             temperatureSetpointNight;
bool            ventilation;
float           temperatureBound;

// wifi password and name definitons
const char*     ssid                = "Luke SkyRouter";
const char*     pass                = "NT7TVT4WS3HAFX";

// device MAC address used for identification
String          MAC_ADDRESS         = "UNSET";

// MQTT server details
const char*     mqtt_server_ip      = "192.168.1.133"; 
int             port                = 1883;
WiFiClient      espClient;
PubSubClient    client(espClient);
 
// epoch time
unsigned long   currentTime         = 0; 
   
// System time variables  
const double    UPDATE_TIMER        = 10; // millimilliseconds

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
const int       TEM_AVG_LENGTH      = 75; 
const int       HUM_AVG_LENGTH      = 50; 
const int       LUX_AVG_LENGTH      = 50; 
  
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

#line 96 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool init_SD_card();
#line 106 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void write_to_config_file(char* payload);
#line 118 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void init_wifi();
#line 126 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void on_message_received(char* topic, byte* message, unsigned int length);
#line 144 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void update_relevant_setpoint(char* data);
#line 157 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void encode_to_json(char* payload);
#line 180 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool load_configuration_file();
#line 203 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void reconnect();
#line 215 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
int get_setpoint();
#line 223 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool is_day(int currentHour);
#line 230 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double calculate_ventilation_wattage(float currentTemperature);
#line 238 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double calculate_air_mass(float temperature);
#line 246 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double calculate_ventilation_T_diff(double joules, double temperature);
#line 250 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
double validate_ventilation_impact_direction(double effect, double temperature);
#line 257 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool is_connected_to_mqtt();
#line 264 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
float sum_mean(float readings[], int length);
#line 272 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool value_changed_enough(float value, float previousValue, float limit);
#line 279 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void transmit_temperature_if_data_changed(float temperature);
#line 306 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void transmit_humidity_if_data_changed(float humidity);
#line 314 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void transmit_lux_if_data_changed(float lux);
#line 321 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void setup();
#line 346 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
void loop();
#line 96 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\deployment\\Device\\device.ino"
bool init_SD_card(){
    SPI.begin(SD_CLK, SD_DO, SD_DI, SD_CS);
    if(!SD.begin(SD_CS)){
        return false;
    }

    file.close(); 
    return true;
}

void write_to_config_file(char* payload){
    if (SD.exists("/config.txt")) {
        SD.remove("/config.txt");
    } 

    file = SD.open("/config.txt", FILE_WRITE);
    if (file){ 
        file.println(payload); 
        file.close();
    } 
}

void init_wifi(){
    WiFi.begin(ssid, pass);
    while(WiFi.status() != WL_CONNECTED){ 
        delay(500);
    }
    MAC_ADDRESS = WiFi.macAddress();
}

void on_message_received(char* topic, byte* message, unsigned int length) {
    char payload[length];
    for (int i=0;i<length;i++)
    {
        payload[i] = (char)message[i];
    }  
    if (((String) topic) == (MAC_ADDRESS+"/room/config")){
        if (SD_CARD_AVAILABLE){
        write_to_config_file(payload); 
        }
        encode_to_json(payload);
    } else if (((String) topic) == (MAC_ADDRESS+"/setpoint")){
        update_relevant_setpoint(payload);
    }
    Serial.println("received:");
    Serial.println(payload); 
}
 
void update_relevant_setpoint(char* data){
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

void encode_to_json(char* payload){
    if (deserializeJson(config, payload)) {
        return;
    }
    const char* room = config["room"];
    unsigned int rWidth = config["roomWidth"];
    unsigned int rHeight = config["roomHeight"];
    unsigned int rLength = config["roomLength"];
    unsigned int tDay = config["tDay"];
    unsigned int tNight = config["tNight"];
    bool vent = config["ventilation"];
    float bound = config["tBound"];
    roomName = room;
    roomHeight = rHeight; 
    roomWidth  = rWidth;
    roomLength = rLength;
    temperatureSetpointDay = tDay;
    temperatureSetpointNight = tNight; 
    temperatureBound = bound;
    ventilation = vent;
    loadedConfig = true; 
}

bool load_configuration_file(){
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
            encode_to_json(content); 
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

int get_setpoint(){
    if (daytime){
        return temperatureSetpointDay;
    } else {
        return temperatureSetpointNight;
    }
}

bool is_day(int currentHour){
    if (currentHour > 6 && currentHour < 21){
        return true;
    } 
    return false; 
}
 
double calculate_ventilation_wattage(float currentTemperature){
    if (currentTemperature > get_setpoint()) { 
        return ((currentTemperature - get_setpoint()) * 8.4) * 100; 
    } else {
        return ((get_setpoint() - currentTemperature) * 8.4) * 100;
    }
}
 
double calculate_air_mass(float temperature){
    double mass =  (101.325*1000) / (287.058 * (temperature + 273.15)) * (roomHeight * roomLength * roomWidth);
    if (mass < 0){ 
        mass = mass * -1; 
    }
    return mass;  
}
  
double calculate_ventilation_T_diff(double joules, double temperature){
    return (joules / calculate_air_mass(temperature)) / 1012;
} 

double validate_ventilation_impact_direction(double effect, double temperature){
    if (temperature > get_setpoint()){
        return -effect;
    } 
    return effect;
}

bool is_connected_to_mqtt(){
    if (!client.connected()) {
        reconnect();
    }  
    return true;
} 
 
float sum_mean(float readings[], int length){ 
    float summation = 0;   
    for (int i = 0; i < length; i++){
        summation += readings[i];
    }
    return summation/length;
}

bool value_changed_enough(float value, float previousValue, float limit){
    if (value > previousValue+limit || value < previousValue-limit){ 
        return true;
    } 
    return false; 
}

void transmit_temperature_if_data_changed(float temperature){

    venWattage = 0;
    if (ventilation){
        temperature += venCelcius;  
        venWattage = calculate_ventilation_wattage(temperature);  
        venDiff = calculate_ventilation_T_diff(venWattage * (UPDATE_TIMER/1000), temperature);
        venDiff = validate_ventilation_impact_direction(venDiff, temperature);
        venCelcius += venDiff;
        temperature += venDiff; 
        Serial.println(lastReportedTem+TEMP_CHANGE_BOUND, 1);
    } 
   
    currentTime = timeNTPClient.getEpochTime(); 
    if (value_changed_enough(temperature, lastReportedTem, TEMP_CHANGE_BOUND)){
        client.publish((MAC_ADDRESS+"/temperature").c_str(), (((String) temperature)+","+roomName+","+((String)currentTime)+","+((String)tempOldestIndex)).c_str());
        lastReportedTem = temperature;
    }  
    if (value_changed_enough(venWattage, lastReportedVen, 100)){
        client.publish((MAC_ADDRESS+"/ventilation").c_str(), (((String) venWattage)+","+roomName+","+((String)currentTime)).c_str());
        lastReportedVen = venWattage;  
    }   
    if (value_changed_enough(temperature, get_setpoint(), temperatureBound)){
        client.publish("sensor/alert", ("room="+roomName+", temperature="+(String) temperature).c_str());
    }
}

void transmit_humidity_if_data_changed(float humidity){ 
    if (value_changed_enough(humidity, lastReportedHum, HUMI_CHANGE_BOUND)){
        client.publish((MAC_ADDRESS+"/humidity").c_str(), (((String) humidity)+","+roomName+","+((String)currentTime)+","+((String)humiOldestIndex)).c_str());
        lastReportedHum = humidity;
    }
 
}

void transmit_lux_if_data_changed(float lux){ 
    if (value_changed_enough(lux, lastReportedLux, LUX_CHANGE_BOUND)){
        client.publish((MAC_ADDRESS+"/lux").c_str(), (((String) lux)+","+roomName+","+((String)currentTime)+","+((String)luxOldestIndex)).c_str());
        lastReportedLux = lux; 
    }  
}

void setup() {
    Serial.begin(9600);  
    delay(1);
 
    if (init_SD_card()){ 
        load_configuration_file();
        SD_CARD_AVAILABLE = true; 
    }

    init_wifi();
    timeNTPClient.begin();
    initSensors();

    client.setServer(mqtt_server_ip, port);
    client.setCallback(on_message_received);

    if (is_connected_to_mqtt()){  
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
        systemTime = milliseconds();
        if (systemTime - lastUpdate >= UPDATE_TIMER){
            timeNTPClient.update(); 
            currentHour = timeNTPClient.getHours()+2;  
            daytime = is_day(currentHour);

            temperatureAvg[tempOldestIndex]  = get_temperature();
            humidityAvg[humiOldestIndex]     = get_humidity();
            lightAvg[luxOldestIndex]         = get_ambientLight(); 
            
            if (tempRunning){
                transmit_temperature_if_data_changed(sum_mean(temperatureAvg, TEM_AVG_LENGTH));
            }
            if (humRunning){
                transmit_humidity_if_data_changed(sum_mean(humidityAvg, HUM_AVG_LENGTH));
            }
            if (luxRunning){
                transmit_lux_if_data_changed(sum_mean(lightAvg, LUX_AVG_LENGTH)); 
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

            timeTaken = milliseconds(); 
            lastUpdate = timeTaken - (systemTime - timeTaken); 
        }
    }
}

