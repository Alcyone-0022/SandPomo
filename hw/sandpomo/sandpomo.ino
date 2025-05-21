#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MPU6050_light.h>

#define DEBUG false
#define LED_PIN     8
#define NUM_LEDS    8
#define MAX_BRIGHTNESS 30

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
MPU6050 mpu(Wire);

unsigned long prevMillis = 0;
unsigned long pomodoroTime = 0.5*60*1000;
unsigned long restingTime = 0.5*60*1000;
unsigned long timeLeft = pomodoroTime;
const uint16_t fadeInterval = 10; // 밝기 갱신 주기(ms)

bool isTimeUp = false;

byte upperLedVal = MAX_BRIGHTNESS;
byte middle2LedVal = MAX_BRIGHTNESS;
byte middle1LedVal = MAX_BRIGHTNESS;
byte lowerLedVal = MAX_BRIGHTNESS;

// 색상 모드 열거형
enum ColorMode { RED, GREEN };
ColorMode modeNow = RED;

enum upDownState { UP, DOWN };
upDownState posNow;
upDownState posPrev;

enum angleState { VERTICAL, HORIZONTAL };
angleState angleNow = VERTICAL;

bool upsideDown = false;
bool tick = false; // for blinking led when usb connected

//**** Forwards ****
void setLED(byte upper, byte mid2, byte mid1, byte lower, bool reverse, ColorMode colorMode);
void setLEDYellow();
void setLEDRed();
void setLEDVal(unsigned long timerNow, unsigned long timeLeft);
upDownState getPosNow();
angleState getAngleNow();
bool isUpsideDown();

void setup() {
  Serial.begin(2000000);
  Wire.setPins(3, 4);
  Wire.begin();
  strip.begin();
  strip.show();

  byte status = mpu.begin();
  while(status != 0){ 
    status = mpu.begin();
    Serial.println("NO MPU FOUND!");
    delay(100);
  } // 센서 연결 확인
  // mpu.calcOffsets();  // 자이로/가속도계 오프셋 계산

  setLEDYellow();

  // for initial calibration of MPU
  prevMillis = millis();
  while (millis() - prevMillis <= 1000) {
    mpu.update();
  }

  posNow = getPosNow();
  posPrev = getPosNow();
  setLED(upperLedVal, middle2LedVal, middle1LedVal, lowerLedVal, (posNow == UP) ? true : false, modeNow);
  
  // stop time till fliped when initialized
  while (posPrev == posNow) {
    posNow = getPosNow();
    Serial.println((posNow == UP) ? "UP" : "DOWN");
    delay(100);
  }
  
}

void loop() {
  
  while (Serial) {
    Serial.println(millis());
    if (millis() - prevMillis >= 2000) {
      prevMillis = millis();
      tick = !tick;
    }

    if (tick) {
        setLEDYellow();
      } else {
        strip.clear();
        strip.show();
    }
  }

  if (millis() - prevMillis >= fadeInterval) {
    prevMillis = millis();
    
    if (DEBUG) {
      Serial.print(upperLedVal); Serial.print(" "); Serial.print(middle2LedVal); Serial.print(" "); Serial.print(middle1LedVal); Serial.print(" "); Serial.println(lowerLedVal);
      
      mpu.update();
      float pitch = mpu.getAngleY(); // 보정된 Pitch
      float roll = mpu.getAngleX();  // 보정된 Roll
      Serial.print("Roll: ");
      Serial.print(roll);
      Serial.print("°\tPitch: ");
      Serial.print(pitch);
      Serial.println("°");
  
      // 자세 판별
      if(getAngleNow() == VERTICAL) {
        Serial.println("Upright position");
      } else {
        Serial.println("Lying down (side) position");
      }
  
      // uplight 방향 판별
      if (getPosNow() == UP) {
        Serial.println("UP STATE");
      } else if (getPosNow() == DOWN) {
        Serial.println("DOWN STATE");
      }

      if (isUpsideDown()) {
        Serial.println("UPSIDE DOWN STATE");
      }
    }

    unsigned long timerNow = (modeNow == RED) ? pomodoroTime : restingTime;
  
    // Control timer and current mode at this section
    if (timeLeft - fadeInterval >= fadeInterval && !isTimeUp) {
      isTimeUp = false;

      setLEDVal(timerNow, timeLeft);
      posNow = getPosNow();
      setLED(upperLedVal, middle2LedVal, middle1LedVal, lowerLedVal, (posNow == UP) ? false : true, modeNow);

      // if lying down side, stop timer
      if (getAngleNow() == HORIZONTAL) {
        setLEDYellow();
        Serial.println("TIME STOPPED");
      } else if (isUpsideDown()){
        // if upside down, initialize timer
        timerNow = pomodoroTime;
        timeLeft = timerNow;
        modeNow = RED;
        setLEDRed();
        Serial.println("TIME INITIALIZED");
      } else {
        // elapse current timer
        timeLeft = timeLeft - fadeInterval;
      }
    } else {
      // TIME IS UP!

      // initialize timer once!
      if (!isTimeUp) {
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

        posPrev = getPosNow();
        isTimeUp = true;
      }
      
      // stop time till fliped
      if (posPrev == posNow) {
        posNow = getPosNow();
        // well... actually this delay will not block main loop.
        delay(100);
      }

      // exit timeup state (end for waiting flip)
      if (posPrev != posNow) {
        isTimeUp = false;
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

void setLEDYellow() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 20, 20, 0, 0);
  }
  strip.show();
}

void setLEDRed() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 20, 0, 0, 0);
  }
  strip.show();
}

void setLEDVal(unsigned long timerNow, unsigned long timeLeft) {
  // calculate and set led values
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

bool isUpsideDown() {
  mpu.update();
  if (abs(mpu.getAngleX()) > 160) {
    return true;
  } else {
    return false;
  }
}