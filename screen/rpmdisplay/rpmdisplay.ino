
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeSans9pt7b.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
#define ROTATION 1
#define BLUE 0x0005
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

uint16_t rpm= 0;
uint16_t nrpm = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("FSAE display"); 
 
  tft.begin();
  randomSeed(analogRead(0));

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  tft.fillScreen(BLUE);
  tft.setRotation(ROTATION);
  tft.setFont(&FreeSans9pt7b);
  tft.setCursor(tft.width() / 5, tft.height() / 3);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("RPMS:"); writeRPMs(rpm);
}

void loop() {
  if (rpm < 15000) {
    if (nrpm > 25) {
      nrpm += random(100) - 25;
    } else {
      nrpm += random(75);
    }
  } else {
    nrpm = 0;
  }
  if (int(nrpm) - int(rpm) > 25 || int(rpm) - int(nrpm) > 25 ) {
    writeRPMs(nrpm);
  }   
  rpm = nrpm;
  delay(50);
}

unsigned long writeRPMs(uint16_t rpm) {
  unsigned long start = micros();
  int cursorX = tft.width() / 2 + 20;
  int cursorY = tft.height() / 3;
  tft.fillRect(cursorX, cursorY-25, 200, 27, BLUE);
  tft.setCursor(cursorX, cursorY);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(rpm);
  return micros() - start;
}
