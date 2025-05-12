#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define LED_PIN     8
#define NUM_LEDS    8
#define MAX_BRIGHTNESS 20

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long prevMillis = 0;
unsigned long pomodoroTime = 5*60*1000;
unsigned long timeLeft = pomodoroTime;
const uint16_t fadeInterval = 50; // 밝기 갱신 주기(ms)
bool direction = false;

//**** Forwards ****
void setLED(byte upper, byte mid2, byte mid1, byte lower, bool reverse);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();
}

void loop() {  
  if (millis() - prevMillis >= fadeInterval) {
    prevMillis = millis();

    byte upperLedVal = MAX_BRIGHTNESS;
    byte middle2LedVal = MAX_BRIGHTNESS;
    byte middle1LedVal = MAX_BRIGHTNESS;
    byte lowerLedVal = MAX_BRIGHTNESS;

    if (timeLeft > pomodoroTime * 3 / 4) {
      // upper LED: 255 → 0
      upperLedVal = map(timeLeft, pomodoroTime, pomodoroTime * 3 / 4, MAX_BRIGHTNESS, 0);
    } else if (timeLeft > pomodoroTime * 2 / 4) {
      // middle2 LED: 255 → 0
      upperLedVal = 0;
      middle2LedVal = map(timeLeft, pomodoroTime * 3 / 4, pomodoroTime * 2 / 4, MAX_BRIGHTNESS, 0);
    } else if (timeLeft > pomodoroTime * 1 / 4) {
      // middle1 LED: 255 → 0
      upperLedVal = 0;
      middle2LedVal = 0;
      middle1LedVal = map(timeLeft, pomodoroTime * 2 / 4, pomodoroTime * 1 / 4, MAX_BRIGHTNESS, 0);
    } else {
      // lower LED: 255 → 0
      upperLedVal = 0;
      middle2LedVal = 0;
      middle1LedVal = 0;
      lowerLedVal = map(timeLeft, pomodoroTime * 1 / 4, 0, MAX_BRIGHTNESS, 0);
    }

    Serial.print(upperLedVal); Serial.print(" "); Serial.print(middle2LedVal); Serial.print(" "); Serial.print(middle1LedVal); Serial.print(" "); Serial.println(lowerLedVal);
    setLED(upperLedVal, middle2LedVal, middle1LedVal, lowerLedVal, direction);

    if (timeLeft - fadeInterval >= fadeInterval) {
      timeLeft = timeLeft - fadeInterval;
    } else {
      timeLeft = pomodoroTime;
      direction = !direction;
    }

  }

}

void setLED(byte upper, byte mid2, byte mid1, byte lower, bool reverse){
  if (!reverse) {
    strip.setPixelColor(0, upper, 0, 0, 0);
    strip.setPixelColor(1, mid2, 0, 0, 0);
    strip.setPixelColor(2, mid1, 0, 0, 0);
    strip.setPixelColor(3, lower, 0, 0, 0);
    strip.setPixelColor(4, MAX_BRIGHTNESS-lower, 0, 0, 0);
    strip.setPixelColor(5, MAX_BRIGHTNESS-mid1, 0, 0, 0);
    strip.setPixelColor(6, MAX_BRIGHTNESS-mid2, 0, 0, 0);
    strip.setPixelColor(7, MAX_BRIGHTNESS-upper,0, 0, 0);
    strip.show();
  } else {
    strip.setPixelColor(7, upper, 0, 0, 0);
    strip.setPixelColor(6, mid2, 0, 0, 0);
    strip.setPixelColor(5, mid1, 0, 0, 0);
    strip.setPixelColor(4, lower, 0, 0, 0);
    strip.setPixelColor(3, MAX_BRIGHTNESS-lower, 0, 0, 0);
    strip.setPixelColor(2, MAX_BRIGHTNESS-mid1, 0, 0, 0);
    strip.setPixelColor(1, MAX_BRIGHTNESS-mid2, 0, 0, 0);
    strip.setPixelColor(0, MAX_BRIGHTNESS-upper,0, 0, 0);
    strip.show();
  }
}
