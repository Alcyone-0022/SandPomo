# 1 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino"
# 2 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino" 2
# 3 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino" 2
# 4 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino" 2
# 5 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino" 2





Adafruit_NeoPixel strip(8, 8, ((1 << 6) | (1 << 4) | (0 << 2) | (2)) /*|< Transmit as G,R,B*/ + 0x0000 /*|< 800 KHz data transmission*/);
MPU6050 mpu(Wire);

unsigned long prevMillis = 0;
unsigned long pomodoroTime = 1*60*1000;
unsigned long restingTime = 0.5*60*1000;
unsigned long timeLeft = pomodoroTime;
const uint16_t fadeInterval = 50; // 밝기 갱신 주기(ms)
bool direction = false;

byte upperLedVal = 20;
byte middle2LedVal = 20;
byte middle1LedVal = 20;
byte lowerLedVal = 20;

// 색상 모드 열거형
enum ColorMode { RED, GREEN };
ColorMode modeNow = RED;

//**** Forwards ****
void setLED(byte upper, byte mid2, byte mid1, byte lower, bool reverse);

void setup() {
  HWCDCSerial.begin(9600);
  Wire.setPins(3, 4);
  Wire.begin();

  byte status = mpu.begin();
  while(status != 0){ delay(10); } // 센서 연결 확인
  mpu.calcOffsets(); // 자이로/가속도계 오프셋 계산

  strip.begin();
  strip.show();
}

void loop() {
  mpu.update();
  float pitch = mpu.getAngleY(); // 보정된 Pitch
  float roll = mpu.getAngleX(); // 보정된 Roll

  if (millis() - prevMillis >= fadeInterval) {
    prevMillis = millis();

    unsigned long timerNow = (modeNow == RED) ? pomodoroTime : restingTime;

    if (timeLeft > timerNow * 3 / 4) {
      // upper LED: 255 → 0
      upperLedVal = map(timeLeft, timerNow, timerNow * 3 / 4, 20, 0);
    } else if (timeLeft > timerNow * 2 / 4) {
      // middle2 LED: 255 → 0
      middle2LedVal = map(timeLeft, timerNow * 3 / 4, timerNow * 2 / 4, 20, 0);
    } else if (timeLeft > timerNow * 1 / 4) {
      // middle1 LED: 255 → 0
      middle1LedVal = map(timeLeft, timerNow * 2 / 4, timerNow * 1 / 4, 20, 0);
    } else {
      // lower LED: 255 → 0
      lowerLedVal = map(timeLeft, timerNow * 1 / 4, 0, 20, 0);
    }

    HWCDCSerial.print(upperLedVal); HWCDCSerial.print(" "); HWCDCSerial.print(middle2LedVal); HWCDCSerial.print(" "); HWCDCSerial.print(middle1LedVal); HWCDCSerial.print(" "); HWCDCSerial.println(lowerLedVal);
    setLED(upperLedVal, middle2LedVal, middle1LedVal, lowerLedVal, direction, modeNow);

    HWCDCSerial.print("Roll: ");
    HWCDCSerial.print(mpu.getAngleX());
    HWCDCSerial.print("°\tPitch: ");
    HWCDCSerial.print(mpu.getAngleY());
    HWCDCSerial.println("°");

    // 자세 판별
    if((abs(roll) < 20 && abs(roll) > 0) || (abs(roll) < 80 && abs(roll) > 50)) {
      HWCDCSerial.println("Upright position");
    } else {
      HWCDCSerial.println("Lying down position");
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
       modeNow = ( modeNow == RED) ? GREEN : RED;

      // reverse dim direction
      direction = !direction;

      // initialize led brightness val
      upperLedVal = 20;
      middle2LedVal = 20;
      middle1LedVal = 20;
      lowerLedVal = 20;
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
      (colorMode == RED) ? (20 - lower) : 0,
      (colorMode == GREEN) ? (20 - lower) : 0,
      0, 0);
    strip.setPixelColor(5,
      (colorMode == RED) ? (20 - mid1) : 0,
      (colorMode == GREEN) ? (20 - mid1) : 0,
      0, 0);
    strip.setPixelColor(6,
      (colorMode == RED) ? (20 - mid2) : 0,
      (colorMode == GREEN) ? (20 - mid2) : 0,
      0, 0);
    strip.setPixelColor(7,
      (colorMode == RED) ? (20 - upper) : 0,
      (colorMode == GREEN) ? (20 - upper) : 0,
      0, 0);
  } else {
    strip.setPixelColor(7, r, g, 0, 0);
    strip.setPixelColor(6, r_mid2, g_mid2, 0, 0);
    strip.setPixelColor(5, r_mid1, g_mid1, 0, 0);
    strip.setPixelColor(4, r_lower, g_lower, 0, 0);
    strip.setPixelColor(3,
      (colorMode == RED) ? (20 - lower) : 0,
      (colorMode == GREEN) ? (20 - lower) : 0,
      0, 0);
    strip.setPixelColor(2,
      (colorMode == RED) ? (20 - mid1) : 0,
      (colorMode == GREEN) ? (20 - mid1) : 0,
      0, 0);
    strip.setPixelColor(1,
      (colorMode == RED) ? (20 - mid2) : 0,
      (colorMode == GREEN) ? (20 - mid2) : 0,
      0, 0);
    strip.setPixelColor(0,
      (colorMode == RED) ? (20 - upper) : 0,
      (colorMode == GREEN) ? (20 - upper) : 0,
      0, 0);
  }
  strip.show();
}
