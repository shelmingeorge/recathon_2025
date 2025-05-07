#include <microLED.h>

const int back_LED_pin = 9, front_LED_pin = 10;
const int LED_amount = 9;

microLED<LED_amount, back_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> back_LED;
microLED<LED_amount, front_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> front_LED;

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


void setup() {
  front_LED.setBrightness(20);
  front_LED.fill(0x0000FF); // blue
  front_LED.show();

  back_LED.setBrightness(20);
  back_LED.fill(0xFF0000); // red
  back_LED.show();

  delay(2000);
  
  for (int i = 0; i < LED_amount; i++){
    front_LED.leds[i] = mRGB(0, 255, 0); //green
    front_LED.show();
    back_LED.leds[i] = mRGB(255, 255, 255); //white
    back_LED.show();
    delay(100);
    }
  

  }

void loop() {
  right_arrors_light(0xFFA500); // orange
  delay(500);
  left_arrors_light(0xFFA500);
  delay(500);
  }
