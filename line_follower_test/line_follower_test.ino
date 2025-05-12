#include <Servo.h>
#include <microLED.h>

const int back_LED_pin = 9, front_LED_pin = 10;
const int LED_amount = 9;

microLED<LED_amount, back_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> back_LED;
microLED<LED_amount, front_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> front_LED;

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
long int start_time = 0;
long int iteration_counter = 0;

void right_arrors_light(int colour){
  back_LED.clear();
  back_LED.fill(7, 8, colour);
  back_LED.fill(2, 4, colour);
  back_LED.setBrightness(20);

  front_LED.clear();
  front_LED.fill(0, 2, colour);
  front_LED.fill(5, 6, colour);
  front_LED.setBrightness(20);

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

bool button_check(){
  bool value = digitalRead(button_pin);
  return value;
  }

void speed_up(int add){
  if (base_speed + add >= 80) return;
  if (base_speed + add <= 20) return;
  base_speed += add;
  return;
  }

void setup() {
  Serial.begin(19200);
  Serial.println("START PROGRAM");

  pinMode(button_pin, INPUT);
  for (int i = A0; i <= A3; i++) pinMode(line_sensor_pins[i], INPUT);

  front_LED.setBrightness(20);
  front_LED.fill(0x0000FF); // blue
  front_LED.show();

  back_LED.setBrightness(20);
  back_LED.fill(0xFF0000); // red
  back_LED.show();

  init_motor(left_motor, left_motor_pin, left_pwm);
  init_motor(right_motor, right_motor_pin, right_pwm);
  move_straight(0);

  while(!button_check()); // ожидание нажатия кнопки
  
  move_straight(base_speed); // заменить на езду прямо по гироскопу
  delay(500);
  start_time = millis();
  }

void loop() {
  if (check_cross_line()) return; // на перекрестке всегда стоять (для теста)
  int left_line = get_line_sensor(A1);
  int right_line = get_line_sensor(A2);

  if (check_cross_line()){
    right_arrors_light(0xFFA500); // orange
    if (cross_lines_counter > 1){
      move_straight(-10);
      delay(100);
      move_straight(0);
      base_speed = 20;
      return; // на перекрестке не ехать
      }
    delay(100);
    right_arrors_light(0x000000);
    cross_lines_counter += 1;
    } 

  follow_line(left_line, right_line);
  if ((iteration_counter % 100 == 0) && ((millis() - start_time) < 25 * 1000)) speed_up(1);
  if ((millis() - start_time) > 25 * 1000) speed_up(-1);
  iteration_counter++;
  }
