#include "Adafruit_NeoPixel.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeSans9pt7b.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
#define ROTATION 1
#define BLUE 0x0005
#define PIN 3
#define shiftup
#define shiftdown

const int SHIFT_UP_BUTTON_PIN = 7;    //subject to change
const int SHIFT_DOWN_BUTTON_PIN = 6;  //subject to change
const int SHIFT_UP_OUTPUT_PIN = 5;    //subject to change
const int SHIFT_DOWN_OUTPUT_PIN = 4;  //subject to change
const int PEAKPOWER_RPM = 13500;
const int MAX_RPM = 15000;
const int IDLE_RPM = 1300;

int serialstate = 0;
int button = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_RGB + NEO_KHZ800);

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

uint16_t rpm = 0;
uint16_t newRPM = 0;
uint16_t prev_gear = 0;
uint16_t current_gear = 0;
uint16_t prevMPH = 0;
uint16_t mph = 0;
bool neutral = true;
int byteRead;

void setup() {
  Serial.begin(9600);
  pinMode(SHIFT_UP_BUTTON_PIN, INPUT);
  pinMode(SHIFT_DOWN_BUTTON_PIN, INPUT);
  pinMode(SHIFT_UP_OUTPUT_PIN, OUTPUT);
  pinMode(SHIFT_DOWN_OUTPUT_PIN, OUTPUT);
  pinMode(PIN, OUTPUT);
  digitalWrite(SHIFT_UP_OUTPUT_PIN, LOW);
  digitalWrite(SHIFT_DOWN_OUTPUT_PIN, LOW);
  //int shift_up = digitalRead(SHIFT_UP_BUTTON_PIN);
  //int shift_down = digitalRead(SHIFT_DOWN_BUTTON_PIN);
  strip.setBrightness(32);
  Serial.println("FSAE display");
  tft.begin();
  randomSeed(analogRead(0));
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  tft.fillScreen(BLUE);
  tft.setRotation(ROTATION);
  tft.setFont(&FreeSans9pt7b);
  //rpm
  writeRPMs(rpm);
  tft.setCursor(230, 120);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("RPM");
  //gear
  writeGear(current_gear);
  tft.setCursor(60, 230);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.print("GEAR");
  //mph
  writeMPH(mph);
  tft.setCursor(230, 230);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.print("MPH");


  //placeholder
  tft.setCursor(30, tft.height() / 3);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1);
  tft.print("TESTING");
  //neutral = true;
}

//bool shifting_up = false;
//bool shifting_down = false;

void loop() {
  int shift_up = digitalRead(SHIFT_UP_BUTTON_PIN);
  int shift_down = digitalRead(SHIFT_DOWN_BUTTON_PIN);
  int neutral_shift = 0;
  //Serial.println(current_gear);
  if (shift_up == HIGH && neutral) {
    Serial.println("shifting 'up' from neutral to 1st");
    neutral = false;
    current_gear = 1;
    neutral_shift = 1;
    digitalWrite(SHIFT_DOWN_OUTPUT_PIN, HIGH);
    delay(10);
    digitalWrite(SHIFT_DOWN_OUTPUT_PIN, LOW);
    delay(500);
  }
  if (shift_down == HIGH && neutral) {
    neutral = false;
    Serial.println("shifting down from neutral to 1st");
    current_gear = 1;
    neutral_shift = 1;
    digitalWrite(SHIFT_DOWN_OUTPUT_PIN, HIGH);
    delay(10);
    digitalWrite(SHIFT_DOWN_OUTPUT_PIN, LOW);
    delay(500);
  }
  if (shift_up == HIGH && !neutral && current_gear < 6 && neutral_shift == 0) {
    Serial.println("normally shifting up");
    current_gear += 1;
    digitalWrite(SHIFT_UP_OUTPUT_PIN, HIGH);
    delay(10);
    digitalWrite(SHIFT_UP_OUTPUT_PIN, LOW);
    delay(500);
    }
  if (shift_down == HIGH && !neutral && current_gear > 1 && neutral_shift == 0) {
    Serial.println("normally shifting down");
    current_gear -= 1;
    digitalWrite(SHIFT_DOWN_OUTPUT_PIN, HIGH);
    delay(10);
    digitalWrite(SHIFT_DOWN_OUTPUT_PIN, LOW);
    delay(500);
    }

  while (Serial.available()) {
    /* read the most recent byte */
    byteRead = Serial.read();     //now byteRead will have latest sensor
    // data sent from the other arduino
  }

  //fake the rpms for now
  if (rpm < 15000) {
    if (newRPM > 25) {
      newRPM += random(100) - 25;
    } else {
      newRPM += random(75);
    }
  } else {
    newRPM = 0;
    //if (random(100) < 20) {
      //current_gear -= 1;
    //} else {
      //current_gear += 1;
    //}
  }
  mph = rpm / 1500 * current_gear;
  if (int(newRPM) - int(rpm) > 25 || int(rpm) - int(newRPM) > 25 ) {
    writeRPMs(newRPM);
  }
  rpm = newRPM;
  delay(50);
  rpmGauge(); //lights up the steering wheel lights

  if (current_gear != prev_gear) {
    writeGear(current_gear);
    prev_gear = current_gear;
  }

  if (mph != prevMPH) {
    writeMPH(mph);
    prevMPH = mph;
  }
}

unsigned long writeRPMs(uint16_t rpm) {
  unsigned long start = micros();
  int cursorX = tft.width() / 2 + 20;
  int cursorY = tft.height() / 3;
  tft.fillRect(cursorX, cursorY - 25, 200, 27, BLUE);
  tft.setCursor(cursorX, cursorY);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(rpm);
  return micros() - start;
}

unsigned long writeGear(uint16_t gear) {
  unsigned long start = micros();
  int cursorX = 65;
  int cursorY = 200;
  tft.setCursor(cursorX, cursorY);
  tft.fillRect(cursorX, cursorY - 37, 130, 50, BLUE);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(3);
  if (neutral) {
    tft.print('N');
  } 
  else {
    tft.print(gear);
  }
  return micros() - start;
}

unsigned long writeMPH(uint16_t mph) {
  unsigned long start = micros();
  int cursorX = 220;
  int cursorY = 200;
  tft.setCursor(cursorX, cursorY);
  tft.fillRect(cursorX, cursorY - 37, 130, 50, BLUE);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(3);
  tft.print(mph);
  return micros() - start;
}
void changePixelState(int index, bool state) {
  if (state) {
    switch (index) {
      case 0:
        strip.setPixelColor(0, 255, 0, 0);
        break;
      case 1:
        strip.setPixelColor(1, 255, 0, 0);
        break;
      case 2:
        strip.setPixelColor(2, 255, 0, 0);
        break;
      case 3:
        strip.setPixelColor(3, 255, 0, 0);
        break;
      case 4:
        strip.setPixelColor(4, 255, 100, 0);
        break;
      case 5:
        strip.setPixelColor(5, 255, 100, 0);
        break;
      case 6:
        strip.setPixelColor(6, 100, 255, 0);
        break;
      case 7:
        strip.setPixelColor(7, 100, 255, 0);
        break;
      case 8:
        strip.setPixelColor(8, 0, 255, 0);
        break;
      case 9:
        strip.setPixelColor(9, 0, 255, 0);
      case 10:
        strip.setPixelColor(10, 0, 255, 0);
        break;
      case 11:
        strip.setPixelColor(11, 0, 255, 0);
        break;
      case 12:
        strip.setPixelColor(12, 0, 0, 255);
        break;
      case 13:
        strip.setPixelColor(13, 0, 0, 255);
        break;
      case 14:
        strip.setPixelColor(14, 0, 0, 255);
        break;
      case 15:
        strip.setPixelColor(15, 0, 0, 255);
        break;
      default:
        Serial.print("invalid");
    }

  } else {
    strip.setPixelColor(index, 0, 0, 0);
  }
  strip.show();
}



void rpmGauge() {
  if (rpm > IDLE_RPM) changePixelState(0, true); else changePixelState(0, false);
  if (rpm > 1900) changePixelState(0, true); else changePixelState(0, false);
  if (rpm > 2000) changePixelState(1, true); else changePixelState(1, false);
  if (rpm > 3000) changePixelState(2, true); else changePixelState(2, false);
  if (rpm > 4000) changePixelState(3, true); else changePixelState(3, false);
  if (rpm > 4500) changePixelState(4, true); else changePixelState(4, false);
  if (rpm > 5000) changePixelState(5, true); else changePixelState(5, false);
  if (rpm > 5500) changePixelState(6, true); else changePixelState(6, false);
  if (rpm > 6000) changePixelState(7, true); else changePixelState(7, false);
  if (rpm > 6500) changePixelState(8, true); else changePixelState(8, false);
  if (rpm > 7000) changePixelState(9, true); else changePixelState(9, false);
  if (rpm > 7500) changePixelState(10, true); else changePixelState(10, false);
  if (rpm > 8000) changePixelState(11, true); else changePixelState(11, false);
  if (rpm > 9000) changePixelState(12, true); else changePixelState(12, false);
  if (rpm > 10000) changePixelState(13, true); else changePixelState(13, false);
  if (rpm > 12000) changePixelState(14, true); else changePixelState(14, false);
  if (rpm > 14000) changePixelState(15, true); else changePixelState(15, false);
}
