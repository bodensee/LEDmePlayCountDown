//
// Count Down Clock
//
// for the LEDmePlay
//
// by Daniel Hirscher 2016
//

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 50
#define LAT 10
#define OE  51
#define A   A0
#define B   A1
#define C   A2
#define D   A3

// Joystick 1
#define buttonU1 30
#define buttonD1 32
#define buttonL1 34
#define buttonR1 36
#define buttonFire1 38

// Audio out
#define audio 2

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);

// start with 18 minutes for the marshmallow challenge
unsigned int minutes = 18;
unsigned long seconds;

unsigned long starttime;
unsigned long endtime;
unsigned long waittime;
unsigned long t;

uint16_t colorStart = matrix.Color333(4, 4, 4);
uint16_t colorMinutes = matrix.Color333(0, 4, 0);
uint16_t colorSeconds = matrix.Color333(0, 1, 1);
uint16_t colorEnd1 = matrix.Color333(7, 0, 0);
uint16_t colorEnd2 = matrix.Color333(0, 0, 0);
uint16_t colorZero = matrix.Color333(4, 0, 0);
uint16_t colorShort = matrix.Color333(4, 4, 0);

boolean end = false;

void setup() {

  // Initialize joystick buttons
  pinMode(buttonU1, INPUT);
  pinMode(buttonD1, INPUT);
  pinMode(buttonL1, INPUT);
  pinMode(buttonR1, INPUT);
  pinMode(buttonFire1, INPUT);

  // Activate internal pull-up resistors 
  digitalWrite(buttonU1, HIGH);
  digitalWrite(buttonD1, HIGH);
  digitalWrite(buttonL1, HIGH);
  digitalWrite(buttonR1, HIGH);
  digitalWrite(buttonFire1, HIGH);
  
  matrix.begin();
}

void loop() {
  welcome();
  prepare();
  countDown();
  alarm();
}

void welcome() {
  matrix.fillScreen(0);
  matrix.setTextSize(1);
  matrix.setTextColor(colorStart);
  matrix.setCursor(1,6);
  matrix.print("Count");
  matrix.setCursor(4,18);
  matrix.print("Down");
  matrix.swapBuffers(false);
  delay(3000);
}

void prepare() {
  setTime();
  boolean fire;
  do {
    if (LOW == digitalRead(buttonU1) && minutes < 99) changeTime(1);
    if (LOW == digitalRead(buttonD1) && minutes > 1) changeTime(-1);
    if (LOW == digitalRead(buttonR1) && minutes < 90) changeTime(10);
    if (LOW == digitalRead(buttonL1) && minutes > 10) changeTime(-10);
    fire = digitalRead(buttonFire1);
  } while(LOW != fire);

  matrix.setTextSize(2);
  matrix.setTextColor(colorMinutes);

  seconds = minutes * 60;
  starttime = millis();
  waittime = starttime + 1000;
  endtime = starttime + (seconds * 1000);
}

void changeTime(int amount) {
  minutes += amount;
  setTime();
  delay(200);
}

void setTime() {
  matrix.fillScreen(0);
  matrix.setTextColor(colorStart);
  matrix.setTextSize(1);
  matrix.setCursor(1, 24);
  matrix.print("Start");
  matrix.setTextSize(2);
  printMinutes(minutes);
  matrix.swapBuffers(false);
}

void countDown() {
  do {
    printTime();
    while (t < waittime) t = millis();
    waittime += 1000;
  } while (t < endtime);
}

void printTime() {
  matrix.fillScreen(0);

  int sa = (millis() - starttime) / 1000;
  int m = minutes - 1 - sa / 60;
  int s = 60 - sa % 60;

  if (m == 0) {
    matrix.setTextColor(colorZero);
  } else if (m < 5) {
    matrix.setTextColor(colorShort);
  }

  // minutes
  printMinutes(m);

  // seconds
  if (s / 2 != 0) {
    matrix.drawLine(29, 30, 29, 31 - s / 2, colorSeconds);
  }
  if ((s + 1) / 2 != 0) {
    matrix.drawLine(30, 30, 30, 31 - (s + 1) / 2, colorSeconds);
  }

  matrix.swapBuffers(false);
}

void printMinutes(int value) {
  if (value < 10) {
    matrix.setCursor(9, 9);
  } else {
    matrix.setCursor(3, 9);
  }
  matrix.print(value);
}

void alarm() {
  boolean fire = HIGH;
  do {
    showAlarm(colorEnd1, colorEnd2, 11);
    fire = digitalRead(buttonFire1);
    if (LOW == fire) break;
    showAlarm(colorEnd2, colorEnd1, 19);
    fire = digitalRead(buttonFire1);
  } while(LOW != fire);
}

void showAlarm(uint16_t colorBackground, uint16_t colorForeground, int pos) {
    matrix.fillScreen(colorBackground);
    matrix.fillCircle(15, 3, 1, colorForeground);
    matrix.fillCircle(15, 10, 6, colorForeground);
    matrix.fillTriangle(15, 2, 3, 24, 27, 24, colorForeground);
    matrix.fillCircle(pos, 26, 4, colorForeground);
    matrix.drawLine(0, 25, 31, 25, colorBackground);
    matrix.swapBuffers(false);
    tone(audio, 200, 200);
    delay(200);
    tone(audio, 1000, 200);
    delay(200);
}

