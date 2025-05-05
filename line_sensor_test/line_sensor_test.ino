const int line_sensor_pins[] = {A0, A1, A2, A3};

int get_line_sensor(int pin){
  int value = analogRead(pin);
  return value;
  }

void setup() {
  Serial.begin(9600);
  Serial.println("START PROGRAM");

  for (int i = A0; i <= A3; i++) pinMode(line_sensor_pins[i], INPUT);
  }

void loop() {
  for (int i = A0; i <= A3; i++){
    Serial.print(i-A0);
    Serial.print("_sensor - ");
    Serial.print(get_line_sensor(i));
    Serial.print("    ");
    }
  Serial.println("");
  delay(100);
  }
