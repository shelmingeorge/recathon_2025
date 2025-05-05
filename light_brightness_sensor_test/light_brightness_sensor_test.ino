#include <BH1750.h>
#include <Wire.h>

const int lux_address = 0x23;
BH1750 lux_sensor(lux_address);

float get_lux_sensor(){
  float lux = lux_sensor.readLightLevel();
  return lux;
  }


void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("START PROGRAM");

  lux_sensor.begin();
  }

void loop() {
  Serial.print("Light: ");
  Serial.print(get_lux_sensor());
  Serial.println(" lx");
  
  delay(300);
}
