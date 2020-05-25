# 1 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\temperature measurement\\Device\\device.ino"
# 2 "c:\\Users\\emilk\\Documents\\IoT\\project\\iot-project\\board\\temperature measurement\\Device\\device.ino" 2

#define seconds() (millis());

double systemTime;
double lastUpdate;
int updateTime = 100;

void setup(){
  systemTime = (millis());;
  lastUpdate = systemTime;

  Serial.begin(9600);

  //initialize sensors
  initialize_sensors();

  Serial.println("temp,light,hum");
}

void loop(){
  systemTime = (millis());;
  double t1 = systemTime - lastUpdate;
  if (t1 >= updateTime){
      float temperature = get_temperature();
      float light = get_ambientLight();
      float humidity = get_humidity();

      Serial.print(temperature);
      Serial.print(",");
      Serial.print(light);
      Serial.print(",");
      Serial.println(humidity);

    double t3 = (millis());;
    lastUpdate = t3 - (systemTime - t3);
  }

}
