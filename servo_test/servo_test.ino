#include <Servo.h>

const int left_motor_pin = 6, right_motor_pin = 5;
const int left_pwm[] = {544, 1540, 2400};
const int right_pwm[] = {544, 1500, 2400};
const int min_pwm = 544, zero_pwm = 1540, max_pwm = 2400;
Servo left_motor, right_motor;

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

void setup() {
  Serial.begin(9600);
  Serial.println("START PROGRAM");
  
  init_motor(left_motor, left_motor_pin, left_pwm);
  init_motor(right_motor, right_motor_pin, right_pwm);

  move_straight(0);

  }

void loop() {
  byte dir = 1;
  for (int i = 20; i <=100; i++){
    move_straight(i*dir);
    delay(100);
    }
  move_straight(0);
  delay(2000);
  dir *= -1;
  }
