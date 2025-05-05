#include <Wire.h>

const int gyro_adress = 0x68;
int axis_speed[] = {0, 0, 0}, axis_accel[] = {0, 0, 0}; // x, y, z
int raw_gyro_data[7];

void gyro_setup(){
  Wire.beginTransmission(gyro_adress);
  Wire.write(gyro_adress);
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  return;
  }

void get_raw_gyro(){
  Wire.beginTransmission(gyro_adress);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(gyro_adress, 14, true); // request a total of 14 registers
  for (byte i = 0; i < 7; i++) raw_gyro_data[i] = Wire.read() << 8 | Wire.read();

  return;
  }

void reform_gyro_data(){
  //тут будут преобразования в нормальные единицы и мб доп расчеты
  raw_gyro_data[0] = axis_speed[0];
  raw_gyro_data[1] = axis_speed[1];
  raw_gyro_data[2] = axis_speed[2];

  raw_gyro_data[4] = axis_accel[0];
  raw_gyro_data[5] = axis_accel[1];
  raw_gyro_data[6] = axis_accel[2];

  return;
  }

void setup() {
  Serial.begin(9600);
  Serial.println("START PROGRAM");
  Wire.begin();
  
  gyro_setup();
  }

void loop() {
  get_raw_gyro();
  reform_gyro_data();

  for (byte i = 0; i < 7; i++) {
    Serial.print(raw_gyro_data[i]);
    Serial.print("  ");
    }
  Serial.println("");
  delay(300);
  }
