#include <Servo.h>
#include "Adafruit_TCS34725.h"
#include <Wire.h>
#include <BH1750.h>
#include "MPU6050.h"
#include "I2Cdev.h"
#include "FastLED.h"

const int left_motor_pin = 5, right_motor_pin = 6;
const int min_pwm = 544, max_pwm = 2400, zero_pwm = 1540;
Servo left_motor, right_motor;

const int line_sensor_pins[] = {A0, A1, A2, A3};
const int button_pin = 2;
int colour_sensor_array[] = {0, 0, 0, 0, 0}; // red, green, blue, colour_temp, lux
int axis_speed[] = {0, 0, 0}, axis_accel[] = {0, 0, 0}; // x, y, z

const int back_LED_pin = 9, front_LED_pin = 10;
const int LED_amount = 9;
CRGB back_LED[LED_amount], front_LED[LED_amount];

Adafruit_TCS34725 colour_sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);
BH1750 lux_sensor(0x23);
MPU6050 accelgyro(0x68);

int get_line_sensor(int pin){
  int value = analogRead(pin);
  return value;
  }

bool button_check(){
  bool value = digitalRead(button_pin);
  return value;
  }

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
float get_lux_sensor(){
  float lux = lux_sensor.readLightLevel();
  return lux;
  }

void get_accelgyro(){
  axis_speed[0] = accelgyro.getRotationX();
  axis_speed[1] = accelgyro.getRotationY();
  axis_speed[2] = accelgyro.getRotationZ();

  axis_accel[0] = accelgyro.getAccelerationX();
  axis_accel[1] = accelgyro.getAccelerationY();
  axis_accel[2] = accelgyro.getAccelerationZ();
  return;
  }
//
void init_motor(Servo motor, int motor_pin){
  motor.attach(motor_pin, min_pwm, max_pwm);
  motor.writeMicroseconds(zero_pwm);
  return;
  }
  
void set_motor_speed(Servo motor, int speed){ // 100 - max speed, -100 - min speed
  int angle_speed = 100 + int((9.0 / 10.0) * speed); // min angle - 0 = min speed, max angle - 180 = max speed

  if (speed > 100) angle_speed = 180;
  if (speed < -100) angle_speed = 0;
  
  motor.write(angle_speed);
  return;
  }

void move_straight(int speed){
  set_motor_speed(left_motor, speed);
  set_motor_speed(right_motor, -speed);
  return;
  }

//
void set_LED_colour(CRGB LED_panel, int colour = 0x00FFFF){
  for (int i = 0; i < LED_amount; i++){
    LED_panel[i] = colour;
    }
  FastLED.show();
  return;
  }
//
float PID_regulation(int value_1, int value_2 = 0){ //это множитель, пока не работает, надо учитывать разность показаний 2х центральных датчиков
  return 1.0;
  }
//
void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("START PROGRAM");

  for (int i = 0; i < 4; i++) pinMode(line_sensor_pins[i], INPUT);
  pinMode(button_pin, INPUT);
  if (!colour_sensor.begin(0x29)) Serial.println("no colour sensor were found");
  if (!lux_sensor.begin()) Serial.println("no lux sensor were found");
  accelgyro.initialize();
  if (!accelgyro.testConnection()) Serial.println("no gyro sensor were found");

  init_motor(left_motor, left_motor_pin);
  init_motor(right_motor, right_motor_pin);

  FastLED.addLeds<WS2812, back_LED_pin, GRB>(back_LED, LED_amount);
  FastLED.addLeds<WS2812, front_LED_pin, GRB>(front_LED, LED_amount);
  FastLED.setBrightness(50);
  }

void loop() {
  set_LED_colour(front_LED, 0x008000);
  move_straight(100);
  delay(2000);
  set_LED_colour(front_LED, 0x000000);

  set_LED_colour(back_LED, 0x008000);
  move_straight(-100);
  delay(2000);
  set_LED_colour(back_LED, 0x000000);
  
  move_straight(0);
  delay(2000);
  }
