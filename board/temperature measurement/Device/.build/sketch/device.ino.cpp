#include <Arduino.h>
#line 1 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\temperature measurement\\Device\\device.ino"
#include "src/sensor_manager.h"

#define seconds()(millis()/1000);

double systemTime;
double lastUpdate;
int updateTime = 0.1; 

#line 9 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\temperature measurement\\Device\\device.ino"
void setup();
#line 21 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\temperature measurement\\Device\\device.ino"
void loop();
#line 9 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\temperature measurement\\Device\\device.ino"
void setup(){ 
  systemTime = seconds();  
  lastUpdate = systemTime;

  Serial.begin(9600); 
 
  //initialize sensors
  initialize_sensors();

  Serial.println("temp,light,hum");
}
 
void loop(){
  systemTime = seconds();
  double t1 = systemTime - lastUpdate;

  if (t1 >= updateTime){
      float temperature = get_temperature();
      float light       = get_ambientLight();
      float humidity    = get_humidity();

      Serial.print(temperature);
      Serial.print(",");
      Serial.print(light);
      Serial.print(",");
      Serial.println(humidity);

    double t3 = seconds();
    lastUpdate = t3 - (systemTime - t3); 
  }

} 
