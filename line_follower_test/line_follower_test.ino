#include <Servo.h>

const int line_sensor_pins[] = {A0, A1, A2, A3};
const int black_border = 300; // верхняя граница, при которой линия считается черной
const int white_border = 800; // нижняя граница, при которой линия считается белой

const int left_motor_pin = 5, right_motor_pin = 6;
const int min_pwm = 544, zero_pwm = 1540, max_pwm = 2400;
const int default_speed = 50; // 50 %
Servo left_motor, right_motor;

int get_line_sensor(int pin){
  int value = analogRead(pin);
  return value;
  }

bool check_cross_line(){
  for (int i = A0; i <= A3; i++){
    if (get_line_sensor(i) > white_border) {
      return false;
      }
    }
  return true;
  }

void init_motor(Servo motor, int motor_pin){
  motor.attach(motor_pin, min_pwm, max_pwm);
  motor.writeMicroseconds(zero_pwm);
  return;
  }
  
void set_motor_speed(Servo motor, int speed){ // 100 - max speed, -100 - min speed
  int pwm_speed = zero_pwm;

  if (speed < 0) pwm_speed = ((zero_pwm - min_pwm) / 100.0) * speed + zero_pwm;
  if (speed >= 0) pwm_speed = ((max_pwm - zero_pwm) / 100.0) * speed + zero_pwm;

  if (speed > 100) pwm_speed = max_pwm;
  if (speed < -100) pwm_speed = min_pwm;
  
  motor.writeMicroseconds(pwm_speed);
  return;
  }

void move_straight(int speed){
  set_motor_speed(left_motor, speed);
  set_motor_speed(right_motor, -speed);
  return;
  }

int regulated_speed(int sensor_data){
  double k = 0.1;
  //тут будет сравнение, k должен зависеть от показаний и мб значений угла гироскопа
  k *= sensor_data; //стандартная скорость 50, диапазон датчика будет от 0 до 1023
  return int(k);
  }

void setup() {
  Serial.begin(115200);
  Serial.println("START PROGRAM");

  for (int i = A0; i <= A3; i++) pinMode(line_sensor_pins[i], INPUT);

  init_motor(left_motor, left_motor_pin);
  init_motor(right_motor, right_motor_pin);
  
  move_straight(0);
  }

void loop() {
  if (check_cross_line()) return; // на перекрестке всегда стоять (для теста)
  int left_line = get_line_sensor(A1);
  int right_line = get_line_sensor(A2);

  set_motor_speed(left_motor, regulated_speed(left_line));
  set_motor_speed(right_motor, regulated_speed(right_line)); // тестовое, возможно вообще по другому делать

  }
