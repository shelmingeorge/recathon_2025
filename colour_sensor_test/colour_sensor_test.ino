#include "Adafruit_TCS34725.h"
#include <Wire.h>

int colour_sensor_array[5]; // red, green, blue, colour_temp, lux
const int colour_sensor_address = 0x29; // по идее ни на что не влияет тк это дефолтный адрес в библиотеке
Adafruit_TCS34725 colour_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

void get_colour_sensor(){ 
  uint16_t r, g, b, c, colour_temp, lux;
 
  colour_sensor.getRawData(&r, &g, &b, &c);
  colour_temp = colour_sensor.calculateColorTemperature_dn40(r, g, b, c);
  lux = colour_sensor.calculateLux(r, g, b); 

  colour_sensor_array[0] = r;
  colour_sensor_array[1] = g;
  colour_sensor_array[2] = b;
  colour_sensor_array[3] = colour_temp;
  colour_sensor_array[4] = lux;
  return;
  }

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("START PROGRAM");

  if(!colour_sensor.begin()) Serial.println("датчик цвета не найден");
}

void loop() {
  get_colour_sensor();

  for (int i = 0; i < 5; i++){
    Serial.print(colour_sensor_array[i]);
    Serial.print("    ");
    if (i == 2) Serial.print("   | дальше не так важно: ");
    }
  Serial.println("");
  delay(100);
}
