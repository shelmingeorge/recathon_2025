#include <Wire.h>

const int gyro_address = 0x68;
int raw_gyro_data[7];
double axis_speed[] = {0.0, 0.0, 0.0}, axis_angle[] = {0.0, 0.0, 0.0}; // x, y, z 
//double axis_accel[] = {0.0, 0.0, 0.0}, axis_linear_speed[] = {0.0, 0.0, 0.0}; // x, y, z // слишком неточно

unsigned long start_time = 0; // микросекунды

void gyro_setup(){
  Wire.beginTransmission(gyro_address);
  Wire.write(0x6B);
  Wire.write(0x00);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  /*
  Wire.beginTransmission(gyro_address);
  Wire.write(0x1C);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(gyro_address);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission();
  */
  start_time = micros();
  return;
  }

void get_raw_gyro(){
  Wire.beginTransmission(gyro_address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(gyro_address, 14, true);
  for (byte i = 0; i < 7; i++) {
    raw_gyro_data[i] = (Wire.read() << 8 | Wire.read());
    }

  return;
  }

void reform_gyro_data(){
  double speed = 250.0; //+-250 град/с диапазон
  double speed_coef = speed / 32768;
  //double g = 9.81; //+-2G диапазон
  //double accel_coef = 2 * g / 32768;

  axis_speed[0] = -1 * (raw_gyro_data[4] * speed_coef + 0.75);
  axis_speed[1] = -1 * (raw_gyro_data[5] * speed_coef - 1.48);
  axis_speed[2] = -1 * (raw_gyro_data[6] * speed_coef + 1.5);

  //axis_accel[0] = raw_gyro_data[0] * accel_coef - 0.95;
  //axis_accel[1] = raw_gyro_data[1] * accel_coef + 0.26;
  //axis_accel[2] = raw_gyro_data[2] * accel_coef + 10.6;

  return;
  }

void get_angles_and_speed(){
  unsigned long time_diff = micros() - start_time;
  if (time_diff < 0) time_diff = micros();
  
  for (byte i = 0; i < 3; i++){
    axis_angle[i] += axis_speed[i] * time_diff / 1000 / 1000;
    if (axis_angle[i] >= 360.0) axis_angle[i] -= 360.0;
    if (axis_angle[i] <= -360.0) axis_angle[i] += 360.0;

    //axis_linear_speed[i] += axis_accel[i] * time_diff / 1000 / 1000;

    }
  start_time = micros();
  return;
  }

void sleep(int miliseconds){
  int time = millis();
  while((millis() - time) < miliseconds);
  return;
  }

void setup() {
  Serial.begin(19200);
  Wire.begin();
  Serial.println("START PROGRAM");
  sleep(1000);
  
  
  gyro_setup();
  }

void loop() {
  get_raw_gyro();
  reform_gyro_data();
  get_angles_and_speed();

  /*
  Serial.print("угл. ск.: ");
  for (byte i = 0; i < 3; i++) {
    Serial.print(axis_speed[i]);
    Serial.print(" град/с| ");
    }

  Serial.print("лин. уск.: ");
  for (byte i = 0; i < 3; i++) {
    Serial.print(axis_accel[i]);
    Serial.print(" м/^2| ");
    }
  */
  Serial.print("углы: ");
  for (byte i = 0; i < 3; i++) {
    Serial.print(axis_angle[i]);
    Serial.print(" град| ");
    }
  /*
  Serial.print("лин. ск.: ");
  for (byte i = 0; i < 3; i++) {
    Serial.print(axis_linear_speed[i]);
    Serial.print(" м/с ");
    }
  */
  
  Serial.println("");
  
  //sleep(20);
  }
