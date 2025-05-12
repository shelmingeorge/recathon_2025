#include <Wire.h>
#include <Servo.h>
#include <microLED.h>
#include <BH1750.h>
#include "Adafruit_TCS34725.h"

const int gyro_address = 0x68;
int raw_gyro_data[7];
double axis_speed[] = {0.0, 0.0, 0.0}, axis_angle[] = {0.0, 0.0, 0.0}; // x, y, z 
//double axis_accel[] = {0.0, 0.0, 0.0}, axis_linear_speed[] = {0.0, 0.0, 0.0}; // x, y, z // слишком неточно
unsigned long start_time = 0; // микросекунды, нужно для интегрирования по угловой скорости

const int back_LED_pin = 9, front_LED_pin = 10;
const int LED_amount = 9;
int base_brightness = 10;
microLED<LED_amount, back_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> back_LED;
microLED<LED_amount, front_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> front_LED;

const int lux_address = 0x23;
BH1750 lux_sensor(lux_address);

int colour_sensor_array[5]; // red, green, blue, colour_temp, lux
const int colour_sensor_address = 0x29;
Adafruit_TCS34725 colour_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_4X);

const int line_sensor_pins[] = {A0, A1, A2, A3};
const int black_border = 500; // верхняя граница, при которой линия считается черной
const int white_border = 800; // нижняя граница, при которой линия считается белой

const int left_motor_pin = 6, right_motor_pin = 5;
const int left_pwm[] = {544, 1480, 2400};
const int right_pwm[] = {544, 1460, 2400};
int base_speed = 20; // регулирование должно быть относительно базовой скорости
Servo left_motor, right_motor;

const int button_pin = 2;

int cross_lines_counter = 0;
unsigned long start_servo_time = 0; // мс
unsigned long iteration_counter = 0;
long int time_per_round = 25 * 1000; // мс

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
  axis_speed[2] = -1 * (raw_gyro_data[6] * speed_coef + 1.57);

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

void right_arrors_light(int colour){
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

void left_arrors_light(int colour){
  front_LED.clear();
  front_LED.fill(7, 8, colour);
  front_LED.fill(2, 4, colour);
  front_LED.setBrightness(20);

  back_LED.clear();
  back_LED.fill(0, 2, colour);
  back_LED.fill(5, 6, colour);
  back_LED.setBrightness(20);

  back_LED.show();
  front_LED.show();
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

int get_line_sensor(int pin){
  int value = analogRead(pin);
  return value;
  }

bool check_cross_line(){
  for (int i = A0; i <= A3; i++){
    if (get_line_sensor(i) > black_border) {
      return false;
      }
    }
  return true;
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

void follow_line(int sensor1_data, int sensor2_data){
  int data_diff = sensor1_data - sensor2_data;
  double k = 0.1;
  int speed_diff = int(k * data_diff); // тут регулирование

  if (speed_diff > base_speed) speed_diff = base_speed; // тут ограничение по регулированию
  if (speed_diff < -base_speed) speed_diff = -base_speed; 

  set_motor_speed(left_motor, left_pwm, base_speed + speed_diff);
  set_motor_speed(right_motor, right_pwm, -(base_speed - speed_diff));
  
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

bool button_check(){
  bool value = digitalRead(button_pin);
  return value;
  }

void speed_up(int add){
  if (base_speed + add >= 80) return;
  if (base_speed + add <= 15) return;
  base_speed += add;
  return;
  }

float get_lux_sensor(){
  float lux = lux_sensor.readLightLevel();
  return lux;
  }

void get_colour_sensor(){
  uint16_t r, g, b, c, colour_temp, lux;
 
  colour_sensor.getRawData(&r, &g, &b, &c);
  //colour_temp = colour_sensor.calculateColorTemperature_dn40(r, g, b, c); // слишком медленно
  //lux = colour_sensor.calculateLux(r, g, b); 

  colour_sensor_array[0] = r;
  colour_sensor_array[1] = g;
  colour_sensor_array[2] = b;
  //colour_sensor_array[3] = colour_temp;
  //colour_sensor_array[4] = lux;
  return;
  }

void setup() {
  Serial.begin(19200);
  Serial.println("START PROGRAM");

  pinMode(button_pin, INPUT);
  for (int i = A0; i <= A3; i++) pinMode(line_sensor_pins[i], INPUT);

  lux_sensor.begin();
  if(!colour_sensor.begin()) Serial.println("датчик цвета не найден");

  front_LED.setBrightness(base_brightness);
  front_LED.fill(0x0000FF); // blue
  front_LED.show();

  back_LED.setBrightness(base_brightness);
  back_LED.fill(0xFF0000); // red
  back_LED.show();

  init_motor(left_motor, left_motor_pin, left_pwm);
  init_motor(right_motor, right_motor_pin, right_pwm);
  move_straight(0);

  while(!button_check()); // ожидание нажатия кнопки
  
  fill_back_colour(0x00FF00); // green
  //move_straight(base_speed); // заменить на езду прямо по гироскопу
  //delay(500);
  gyro_setup();
  start_servo_time = millis();

  sleep(100);
  }

void loop() {
  //get_raw_gyro();
  //reform_gyro_data();
  //get_angles_and_speed();

  //get_colour_sensor();

  if (check_cross_line()) return; // на перекрестке всегда стоять (для теста)
  int left_line = get_line_sensor(A1);
  int right_line = get_line_sensor(A2);

  if (check_cross_line()){
    cross_lines_counter += 1;
    switch (cross_lines_counter)
    {
      case 1: break;
      case 2: 
        move_straight(0);
        base_speed = 20;
        sleep(300);
        return;
      default: break; // тут заполним
      }

    } 
    
  follow_line(left_line, right_line);
  if ((iteration_counter % 100 == 0) && ((millis() - start_servo_time) < time_per_round * 1000)) speed_up(1);
  if ((iteration_counter % 20 == 0) && ((millis() - start_servo_time) >= time_per_round * 1000)) speed_up(-1);
  iteration_counter++;
  }
