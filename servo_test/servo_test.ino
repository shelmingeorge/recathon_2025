#include <Servo.h>

const int left_motor_pin = 5, right_motor_pin = 6;
const int min_pwm = 544, max_pwm = 2400, zero_pwm = 1540;
Servo left_motor, right_motor;

void init_motor(Servo motor, int motor_pin){
  motor.attach(motor_pin, min_pwm, max_pwm);
  motor.writeMicroseconds(zero_pwm);
  return;
  }
  
void set_motor_speed(Servo motor, int speed){ // 100 - max speed, -100 - min speed
  int pwm_speed = zero_pwm;
  if (speed > 100) pwm_speed = max_pwm;
  if (speed < -100) pwm_speed = min_pwm;

  if (speed < 0) pwm_speed = ((zero_pwm - min_pwm) / 100.0) * speed + zero_pwm;
  if (speed >= 0) pwm_speed = ((max_pwm - zero_pwm) / 100.0) * speed + zero_pwm;
  
  motor.writeMicrosecond(pwm_speed);
  return;
  }

void move_straight(int speed){
  set_motor_speed(left_motor, speed);
  set_motor_speed(right_motor, -speed);
  return;
  }

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("START PROGRAM");

  init_motor(left_motor, left_motor_pin);
  init_motor(right_motor, right_motor_pin);

  }

void loop() {
  for(int i = 100; i > 0; i-=20;){
    move_straight(i);
    delay(2000);
    move_straight(-i);
    delay(2000);
    move_straight(0);
    delay(2000);
    }
  }
