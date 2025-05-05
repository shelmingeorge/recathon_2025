const int button_pin = 2;

bool button_check(){
  bool value = digitalRead(button_pin);
  return value;
  }

void setup() {
  Serial.begin(9600);
  Serial.println("START PROGRAM");

  pinMode(button_pin, INPUT);
  }

void loop() {
  if(button_check()) Serial.println("кнопка нажата");
  delay(50);
}
