#include <Wire.h>
#include <Servo.h>
#include <microLED.h>

const int gyro_address = 0x68;
int raw_gyro_data[7];
double axis_speed[] = {0.0, 0.0, 0.0}, axis_angle[] = {0.0, 0.0, 0.0}; // x, y, z 
//double axis_accel[] = {0.0, 0.0, 0.0}, axis_linear_speed[] = {0.0, 0.0, 0.0}; // x, y, z // слишком неточно
unsigned long start_time = 0; // микросекунды, нужно для интегрирования по угловой скорости

unsigned long start_servo_time = 0; // милисекунды, отслеживает время езды 

const int left_motor_pin = 6, right_motor_pin = 5;
const int left_pwm[] = {544, 1480, 2400};
const int right_pwm[] = {544, 1460, 2400};
int base_speed = 20; // регулирование должно быть относительно базовой скорости
Servo left_motor, right_motor;

const int back_LED_pin = 9, front_LED_pin = 10;
const int LED_amount = 9;
int base_brightness = 10;
microLED<LED_amount, back_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> back_LED;
microLED<LED_amount, front_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> front_LED;

const int button_pin = 2;

bool button_check(){
  bool value = digitalRead(button_pin);
  return value;
  }

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

void init_motor(Servo motor, int motor_pin, int pwm_arr[]){
  motor.attach(motor_pin, pwm_arr[0], pwm_arr[2]);
  motor.writeMicroseconds(pwm_arr[1]);
  return;
  }
  
void set_motor_speed(Servo motor, int pwm_arr[], int speed){ // 100 - max speed, -100 - min speed
  int pwm_speed = pwm_arr[1];

  if (speed < 0) pwm_speed = ((pwm_arr[1] - pwm_arr[0]) / 100.0) * speed + pwm_arr[1];
  if (speed >= 0) pwm_speed = ((pwm_arr[2] - pwm_arr[1]) / 100.0) * speed + pwm_arr[1];

  if (speed > 100) pwm_speed = pwm_arr[2];
  if (speed < -100) pwm_speed = pwm_arr[0];

  motor.writeMicroseconds(pwm_speed);
  return;
  }

void move_straight(int speed){
  set_motor_speed(left_motor, left_pwm, speed);
  set_motor_speed(right_motor, right_pwm, -speed);
  return;
  }

void fill_back_colour(long int colour){
  back_LED.setBrightness(base_brightness);
  back_LED.fill(colour);
  back_LED.show();
  return;
  }

void fill_front_colour(long int colour){
  front_LED.setBrightness(base_brightness);
  front_LED.fill(colour);
  front_LED.show();
  return;
  }

void right_arrors_light(long int colour){
  back_LED.clear();
  back_LED.fill(7, 8, colour);
  back_LED.fill(2, 4, colour);
  back_LED.setBrightness(base_brightness);

  front_LED.clear();
  front_LED.fill(0, 2, colour);
  front_LED.fill(5, 6, colour);
  front_LED.setBrightness(base_brightness);

  back_LED.show();
  front_LED.show();
  return;
  }

void left_arrors_light(long int colour){
  front_LED.clear();
  front_LED.fill(7, 8, colour);
  front_LED.fill(2, 4, colour);
  front_LED.setBrightness(base_brightness);

  back_LED.clear();
  back_LED.fill(0, 2, colour);
  back_LED.fill(5, 6, colour);
  back_LED.setBrightness(base_brightness);

  back_LED.show();
  front_LED.show();
  return;
  }

void fix_angle(double angle){
  double angle_diff = angle - axis_angle[2];
  double k = 1.0;
  int speed_diff = int(k * angle_diff); // тут регулирование

  if (speed_diff > 20) speed_diff = 20; // тут ограничение по регулированию
  if (speed_diff < -20) speed_diff = -20; 

  set_motor_speed(left_motor, left_pwm, base_speed - speed_diff);
  set_motor_speed(right_motor, right_pwm, -(base_speed + speed_diff));
  return;
  }

void speed_up(byte add){
  if (base_speed >= 80) return;
  base_speed += add;
  return;
  }

void setup() {
  Serial.begin(19200);
  Wire.begin();
  Serial.println("START PROGRAM");

  pinMode(button_pin, INPUT);

  init_motor(left_motor, left_motor_pin, left_pwm);
  init_motor(right_motor, right_motor_pin, right_pwm);
  move_straight(0);

  fill_back_colour(0xFF0000); // red
  fill_front_colour(0xFFFFFF); // white

  while(!button_check()); // ожидание нажатия кнопки
  
  fill_back_colour(0x000000); // black
  gyro_setup();
  start_servo_time = millis();

  sleep(100);
  move_straight(base_speed);
  }

void loop() {
  unsigned long move_time = 5*1000; //10 секунд ехать прямо

  get_raw_gyro();
  reform_gyro_data();
  get_angles_and_speed();
  Serial.println(axis_angle[2]);
  
  unsigned long current_time = millis() - start_servo_time;

  if(current_time < move_time) {
    fix_angle(0);
    speed_up(1);
    }
  if((current_time > move_time) && (current_time < move_time * 2)){
    fix_angle(180);
    base_speed = 0;
    left_arrors_light(0xFFA500);
    sleep(100);
    left_arrors_light(0x000000);
    sleep(100);
    }
  if(current_time > move_time * 2) {
    base_speed = 0;
    move_straight(0);
    fill_back_colour(0xFF0000); // red
    fill_front_colour(0xFFFFFF); // white
    return; //все что дальше не выполнится по окончанию таймера
    }


  }
