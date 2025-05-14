#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MPU6050_light.h>

#define DEBUG true
#define LED_PIN     8
#define NUM_LEDS    8
#define MAX_BRIGHTNESS 10

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
MPU6050 mpu(Wire);

unsigned long prevMillis = 0;
unsigned long pomodoroTime = 0.5*60*1000;
unsigned long restingTime = 0.5*60*1000;
unsigned long timeLeft = pomodoroTime;
const uint16_t fadeInterval = 50; // 밝기 갱신 주기(ms)

byte upperLedVal = MAX_BRIGHTNESS;
byte middle2LedVal = MAX_BRIGHTNESS;
byte middle1LedVal = MAX_BRIGHTNESS;
byte lowerLedVal = MAX_BRIGHTNESS;

// 색상 모드 열거형
enum ColorMode { RED, GREEN };
ColorMode modeNow = RED;

enum upDownState { UP, DOWN };
upDownState posNow = UP;

enum angleState { VERTICAL, HORIZONTAL };
posState angleNow = VERTICAL;

//**** Forwards ****
void setLED(byte upper, byte mid2, byte mid1, byte lower, bool reverse, ColorMode colorMode);
upDownState getPosNow();

void setup() {
  Serial.begin(9600);
  Wire.setPins(3, 4);
  Wire.begin();
  strip.begin();
  strip.show();

  byte status = mpu.begin();
  while(status != 0){ delay(10); } // 센서 연결 확인
  // mpu.calcOffsets();  // 자이로/가속도계 오프셋 계산

  setLED(upperLedVal, middle2LedVal, middle1LedVal, lowerLedVal, (posNow == UP) ? true : false, modeNow);

  posNow = getPosNow();
  upDownState posPrev = getPosNow();
  // stop time till fliped when initialized
  while (posPrev == posNow) {
    posNow = getPosNow();
    Serial.println((posNow == UP) ? "UP" : "DOWN");
    delay(100);
  }
  
}

void loop() {
  if (millis() - prevMillis >= fadeInterval) {
    prevMillis = millis();

    unsigned long timerNow = (modeNow == RED) ? pomodoroTime : restingTime;

    // calculate led values
    if (timeLeft > timerNow * 3 / 4) {
      // upper LED: 255 → 0
      upperLedVal = map(timeLeft, timerNow, timerNow * 3 / 4, MAX_BRIGHTNESS, -1);
    } else if (timeLeft > timerNow * 2 / 4) {
      // middle2 LED: 255 → 0
      middle2LedVal = map(timeLeft, timerNow * 3 / 4, timerNow * 2 / 4, MAX_BRIGHTNESS, -1);
    } else if (timeLeft > timerNow * 1 / 4) {
      // middle1 LED: 255 → 0
      middle1LedVal = map(timeLeft, timerNow * 2 / 4, timerNow * 1 / 4, MAX_BRIGHTNESS, -1);
    } else {
      // lower LED: 255 → 0
      lowerLedVal = map(timeLeft, timerNow * 1 / 4, 0, MAX_BRIGHTNESS, -1);
    }
    
    posNow = getPosNow();
    setLED(upperLedVal, middle2LedVal, middle1LedVal, lowerLedVal, (posNow == UP) ? false : true, modeNow);

    // mpu.update();
    float pitch = mpu.getAngleY(); // 보정된 Pitch
    float roll = mpu.getAngleX();  // 보정된 Roll


    if (DEBUG) {
      Serial.print(upperLedVal); Serial.print(" "); Serial.print(middle2LedVal); Serial.print(" "); Serial.print(middle1LedVal); Serial.print(" "); Serial.println(lowerLedVal);
      
      Serial.print("Roll: ");
      Serial.print(roll);
      Serial.print("°\tPitch: ");
      Serial.print(pitch);
      Serial.println("°");
  
      // 자세 판별
      if((abs(roll) < 20 && abs(roll) > 0) || (abs(roll) < 80 && abs(roll) > 50)) {
        Serial.println("Upright position");
      } else {
        Serial.println("Lying down position");
      }
  
      // uplight 방향 판별
      if (getPosNow() == UP) {
        Serial.println("UP STATE");
      } else if (getPosNow() == DOWN) {
        Serial.println("DOWN STATE");
      }
    }

    if (timeLeft - fadeInterval >= fadeInterval) {
      timeLeft = timeLeft - fadeInterval;
    } else {
      // TIME IS UP!

      // initialize timer
      if ( modeNow == RED) {
        // colormod now is pomodoro
        // change to resting
        timeLeft = restingTime;
      } else {
        // colormod now is resting
        // change to pomodoro
        timeLeft = pomodoroTime;
      }

      // change colormod
       modeNow = (modeNow == RED) ? GREEN : RED;

      // initialize led brightness val
      upperLedVal = MAX_BRIGHTNESS;
      middle2LedVal = MAX_BRIGHTNESS;
      middle1LedVal = MAX_BRIGHTNESS;
      lowerLedVal = MAX_BRIGHTNESS;

      // change led color to opposite
      setLED(upperLedVal, middle2LedVal, middle1LedVal, lowerLedVal, (posNow == UP) ? true : false, modeNow);

      upDownState posPrev = getPosNow();
      // stop time till fliped
      while (posPrev == posNow) {
        posNow = getPosNow();
        delay(50);
      }
    }
  }
}

void setLED(byte upper, byte mid2, byte mid1, byte lower, bool reverse, ColorMode colorMode) {
  uint8_t r = (colorMode == RED) ? upper : 0;
  uint8_t g = (colorMode == GREEN) ? upper : 0;
  uint8_t r_mid2 = (colorMode == RED) ? mid2 : 0;
  uint8_t g_mid2 = (colorMode == GREEN) ? mid2 : 0;
  uint8_t r_mid1 = (colorMode == RED) ? mid1 : 0;
  uint8_t g_mid1 = (colorMode == GREEN) ? mid1 : 0;
  uint8_t r_lower = (colorMode == RED) ? lower : 0;
  uint8_t g_lower = (colorMode == GREEN) ? lower : 0;

  if (!reverse) {
    strip.setPixelColor(0, r, g, 0, 0);
    strip.setPixelColor(1, r_mid2, g_mid2, 0, 0);
    strip.setPixelColor(2, r_mid1, g_mid1, 0, 0);
    strip.setPixelColor(3, r_lower, g_lower, 0, 0);
    strip.setPixelColor(4, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - lower) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - lower) : 0, 
      0, 0);
    strip.setPixelColor(5, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - mid1) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - mid1) : 0, 
      0, 0);
    strip.setPixelColor(6, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - mid2) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - mid2) : 0, 
      0, 0);
    strip.setPixelColor(7, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - upper) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - upper) : 0, 
      0, 0);
  } else {
    strip.setPixelColor(7, r, g, 0, 0);
    strip.setPixelColor(6, r_mid2, g_mid2, 0, 0);
    strip.setPixelColor(5, r_mid1, g_mid1, 0, 0);
    strip.setPixelColor(4, r_lower, g_lower, 0, 0);
    strip.setPixelColor(3, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - lower) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - lower) : 0, 
      0, 0);
    strip.setPixelColor(2, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - mid1) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - mid1) : 0, 
      0, 0);
    strip.setPixelColor(1, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - mid2) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - mid2) : 0, 
      0, 0);
    strip.setPixelColor(0, 
      (colorMode == RED) ? (MAX_BRIGHTNESS - upper) : 0,
      (colorMode == GREEN) ? (MAX_BRIGHTNESS - upper) : 0, 
      0, 0);
  }
  strip.show();
}

upDownState getPosNow() {
  mpu.update();
  // uplight 방향 판별
  if (mpu.getAngleX() < -80 && abs(mpu.getAngleY()) < 10) {
    return UP;
  } else if (mpu.getAngleX() > 80 && abs(mpu.getAngleY()) < 10) {
    return DOWN;
  }
}

angleState getAngleNow() {
  mpu.update();
  if (abs(mpu.getAngleY()) > 80) {
    return HORIZONTAL;
  } else {
    return VERTICAL;
  }
}