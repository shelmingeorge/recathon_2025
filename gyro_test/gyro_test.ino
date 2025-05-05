#include <Wire.h>

const int gyro_address = 0x68;
double axis_speed[] = {0, 0, 0}, axis_accel[] = {0, 0, 0}; // x, y, z
int raw_gyro_data[7];

void gyro_setup(){
  Wire.beginTransmission(gyro_address);
  Wire.write(gyro_address);
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  return;
  }

void get_raw_gyro(){
  Wire.beginTransmission(gyro_address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(gyro_address, 14, true); // request a total of 14 registers
  for (byte i = 0; i < 7; i++) raw_gyro_data[i] = Wire.read() << 8 | Wire.read();

  return;
  }

void reform_gyro_data(){
  double g = 9.81; //+-2G диапазон
  double speed = 250.0; //+-250 град/с диапазон
  double accel_coef = 2 * g / 32768;
  double speed_coef = speed / 32768;

  axis_speed[0] = raw_gyro_data[4] * speed_coef;
  axis_speed[1] = raw_gyro_data[5] * speed_coef;
  axis_speed[2] = raw_gyro_data[6] * speed_coef;

  axis_accel[0] = raw_gyro_data[0] * accel_coef;
  axis_accel[1] = raw_gyro_data[1] * accel_coef;
  axis_accel[2] = raw_gyro_data[2] * accel_coef;

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

  Serial.print("угловые скорости: ");
  for (byte i = 0; i < 3; i++) {
    Serial.print(axis_speed[i]);
    Serial.print(" град/с   ");
    }

  Serial.print("угловые ускорения: ");
  for (byte i = 0; i < 3; i++) {
    Serial.print(axis_accel[i]);
    Serial.print(" G   ");
    }

  Serial.println("");
  delay(100);
  }
