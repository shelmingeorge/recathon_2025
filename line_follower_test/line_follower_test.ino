#include <Servo.h>

const int line_sensor_pins[] = {A0, A1, A2, A3};
const int black_border = 300; // верхняя граница, при которой линия считается черной
const int white_border = 800; // нижняя граница, при которой линия считается белой

const int left_motor_pin = 6, right_motor_pin = 5;
const int left_pwm[] = {544, 1480, 2400};
const int right_pwm[] = {544, 1460, 2400};
int base_speed = 20; // регулирование должно быть относительно базовой скорости
Servo left_motor, right_motor;

const int button_pin = 2;

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
  double k = 0.6;
  int speed_diff = int(k * data_diff); // тут регулирование

  if (speed_diff > base_speed) speed_diff = base_speed; // тут ограничение по регулированию
  if (speed_diff < -base_speed) speed_diff = -base_speed; 

  set_motor_speed(left_motor, left_pwm, base_speed - speed_diff);
  set_motor_speed(right_motor, right_pwm, -(base_speed + speed_diff));
  
  return;
  }

bool button_check(){
  bool value = digitalRead(button_pin);
  return value;
  }

void speed_up(byte add){
  if (base_speed >= 80) return;
  base_speed += add;
  return;
  }

void setup() {
  Serial.begin(115200);
  Serial.println("START PROGRAM");

  pinMode(button_pin, INPUT);
  for (int i = A0; i <= A3; i++) pinMode(line_sensor_pins[i], INPUT);

  init_motor(left_motor, left_motor_pin, left_pwm);
  init_motor(right_motor, right_motor_pin, right_pwm);
  move_straight(0);

  while(!button_check()); // ожидание нажатия кнопки
  }

void loop() {
  if (check_cross_line()) return; // на перекрестке всегда стоять (для теста)
  int left_line = get_line_sensor(A1);
  int right_line = get_line_sensor(A2);

  if (check_cross_line()){
    move_straight(0);
    return; // на перекрестке не ехать
    } 

  follow_line(left_line, right_line);
  speed_up(1);
  }
