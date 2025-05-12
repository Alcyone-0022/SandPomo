# 1 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino"
# 2 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino" 2
# 3 "C:\\Users\\Seunggab Ha\\Desktop\\SandPomo\\hw\\sandpomo\\sandpomo.ino" 2





Adafruit_NeoPixel strip(8, 8, ((1 << 6) | (1 << 4) | (0 << 2) | (2)) /*|< Transmit as G,R,B*/ + 0x0000 /*|< 800 KHz data transmission*/);

unsigned long prevMillis = 0;
unsigned long pomodoroTime = 5*60*1000;
unsigned long timeLeft = pomodoroTime;
const uint16_t fadeInterval = 50; // 밝기 갱신 주기(ms)
bool direction = false;

//**** Forwards ****
void setLED(byte upper, byte mid2, byte mid1, byte lower, bool reverse);

void setup() {
  HWCDCSerial.begin(9600);
  strip.begin();
  strip.show();
}

void loop() {
  if (millis() - prevMillis >= fadeInterval) {
    prevMillis = millis();

    byte upperLedVal = 20;
    byte middle2LedVal = 20;
    byte middle1LedVal = 20;
    byte lowerLedVal = 20;

    if (timeLeft > pomodoroTime * 3 / 4) {
      // upper LED: 255 → 0
      upperLedVal = map(timeLeft, pomodoroTime, pomodoroTime * 3 / 4, 20, 0);
    } else if (timeLeft > pomodoroTime * 2 / 4) {
      // middle2 LED: 255 → 0
      upperLedVal = 0;
      middle2LedVal = map(timeLeft, pomodoroTime * 3 / 4, pomodoroTime * 2 / 4, 20, 0);
    } else if (timeLeft > pomodoroTime * 1 / 4) {
      // middle1 LED: 255 → 0
      upperLedVal = 0;
      middle2LedVal = 0;
      middle1LedVal = map(timeLeft, pomodoroTime * 2 / 4, pomodoroTime * 1 / 4, 20, 0);
    } else {
      // lower LED: 255 → 0
      upperLedVal = 0;
      middle2LedVal = 0;
      middle1LedVal = 0;
      lowerLedVal = map(timeLeft, pomodoroTime * 1 / 4, 0, 20, 0);
    }

    HWCDCSerial.print(upperLedVal); HWCDCSerial.print(" "); HWCDCSerial.print(middle2LedVal); HWCDCSerial.print(" "); HWCDCSerial.print(middle1LedVal); HWCDCSerial.print(" "); HWCDCSerial.println(lowerLedVal);
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
    strip.setPixelColor(4, 20 -lower, 0, 0, 0);
    strip.setPixelColor(5, 20 -mid1, 0, 0, 0);
    strip.setPixelColor(6, 20 -mid2, 0, 0, 0);
    strip.setPixelColor(7, 20 -upper,0, 0, 0);
    strip.show();
  } else {
    strip.setPixelColor(7, upper, 0, 0, 0);
    strip.setPixelColor(6, mid2, 0, 0, 0);
    strip.setPixelColor(5, mid1, 0, 0, 0);
    strip.setPixelColor(4, lower, 0, 0, 0);
    strip.setPixelColor(3, 20 -lower, 0, 0, 0);
    strip.setPixelColor(2, 20 -mid1, 0, 0, 0);
    strip.setPixelColor(1, 20 -mid2, 0, 0, 0);
    strip.setPixelColor(0, 20 -upper,0, 0, 0);
    strip.show();
  }
}
