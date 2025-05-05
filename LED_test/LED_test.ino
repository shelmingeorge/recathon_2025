#include <microLED.h>

const int back_LED_pin = 9, front_LED_pin = 10;
const int LED_amount = 9;

microLED<LED_amount, back_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> back_LED;
microLED<LED_amount, front_LED_pin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> front_LED;

void setup() {
  front_LED.setBrightness(50);
  front_LED.fill(0x0000FF);
  front_LED.show();

  back_LED.setBrightness(50);
  back_LED.fill(0xFF8000);
  back_LED.show();

  delay(2000);

  for (int i == 0; i < LED_amount; i++){
    front_LED.leds[i] = mRGB(0, 255, 0);
    front_LED.show();
    back_LED.leds[i] = mRGB(255, 255, 255);
    back_LED.show();
    delay(500);
    }

  }

void loop() {
  }
