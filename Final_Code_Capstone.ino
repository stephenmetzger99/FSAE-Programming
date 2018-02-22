#include <Adafruit_LEDBackpack.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"


#include <Servo.h>

// Declaring button pins to use on arduino
#define BUTTON_DOWN  11
#define  BUTTON_UP   13
#define   BUTTON_N   12   
#define   ECU_PIN    8
#define  val_DOWN    5
#define  val_UP      6
#define servo_pin    3
//#define LAUNCH_pin   11
//#define PWM_pin      6
//#define pot_pin      A1
//#define encode_pin   9

// Variables for MPH
//uint8_1 ones, tens;

// Setting initial old states of buttons as high since they are pulled UP
bool   oldState_UP = HIGH; 
bool oldState_DOWN = HIGH; 
bool    oldState_N = HIGH; 

//double DC_time, DC_MPH;

int showGear;                            // Gear indicator

Adafruit_7segment matrix = Adafruit_7segment();

// Declaring servo motor
Servo clutch_servo;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_UP,   INPUT_PULLUP);    // So activate when button goes low
  pinMode(BUTTON_DOWN, INPUT_PULLUP);    // so activate when button goes low
  pinMode(BUTTON_N,    INPUT_PULLUP);    // so activate when button goes low
  pinMode(ECU_PIN,           OUTPUT);
  pinMode(val_UP,            OUTPUT);
  pinMode(val_DOWN,          OUTPUT);
//  pinMode(PWM_pin,           OUTPUT);
//  pinMode(encode_pin,  INPUT_PULLUP);

  clutch_servo.attach(servo_pin);        // activate servo control
  clutch_servo.write(117);               // set servo to mid-point

  matrix.begin(0x70);                    // initializes display
  showGear=2;                            // Gear display starts in neutral

  // Clear screen on startup
  matrix.writeDigitRaw(0,B00000000); matrix.writeDigitRaw(1,B00000000);
  matrix.writeDigitRaw(3,B00000000); matrix.writeDigitRaw(4,B00000000);
  matrix.writeDisplay(); delay(500);

  // Display startup
  matrix.writeDigitRaw(0,B11000000);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(1,B11000000);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(0,B10000000);matrix.writeDigitRaw(3,B11000000);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(1,B10000000);matrix.writeDigitRaw(4,B11000000);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(3,B10000000);matrix.writeDigitRaw(4,B11000110);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(4,B10001111);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(4,B10001001);matrix.writeDigitRaw(3,B10001001);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(4,B10000000);matrix.writeDigitRaw(1,B10001001);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(3,B10000000);matrix.writeDigitRaw(0,B10001001);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(1,B10000000);matrix.writeDigitRaw(0,B10110000);matrix.writeDisplay(); delay(100);
  matrix.writeDigitRaw(0,B10000000);matrix.writeDisplay(); delay(100);

  
  // Display FSAE on startup
  matrix.writeDigitRaw(0,B11110001);matrix.writeDisplay(); delay(500);
  matrix.writeDigitRaw(1,B11101101);matrix.writeDisplay(); delay(500);
  matrix.writeDigitRaw(3,B11110111);matrix.writeDisplay(); delay(500);
  matrix.writeDigitRaw(4,B11111001);matrix.writeDisplay(); delay(500);
  matrix.writeDisplay();
  delay(1500);

 // Clear screen and start at 0 MPH and in neutral
  matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
  matrix.writeDigitRaw(4,B11000000); matrix.writeDigitRaw(3,B10000000);
  matrix.writeDisplay();
}

void loop() {
  // Get current button state.
  bool newState_UP = digitalRead(BUTTON_UP); 
  bool newState_DOWN=digitalRead(BUTTON_DOWN);
  bool newState_N = digitalRead(BUTTON_N);
//  bool launch = digitalRead(LAUNCH_pin);


  
  /////////////////////////////////////////////////////////////////
  //                          UPSHIFT                            //
  /////////////////////////////////////////////////////////////////
    
  if (showGear<6){          // Valves and ECU will not fire past 6th gear in upshift
    
    if (newState_UP == LOW && oldState_UP == HIGH) {  //Check to see if button state has changed
      delay(20);                                      // Short delay to debounce button.
      newState_UP = digitalRead(BUTTON_UP);           // Check if button is still low after debounce.
      if (newState_UP == LOW) {
        if (showGear==0){           
          showGear=2;                                 // SKIP neutral
          }                  
        else {
          showGear++;                                 // adding to gear number
       }
       startShift(showGear); Serial.print(val_UP);
       digitalWrite(ECU_PIN, HIGH);
       matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0); 
       matrix.writeDigitRaw(3,B10100011); matrix.writeDisplay();
       delay(20);                                     // So that ECU has to time react
       digitalWrite(val_UP, HIGH);Serial.print(val_UP);
       delay(120);                                    // time for shift to complete
       digitalWrite(ECU_PIN, LOW);
       digitalWrite(val_UP, LOW);Serial.print(val_UP);
       matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
       matrix.writeDigitRaw(3,B00000000); matrix.writeDisplay();
       }
      } 
  }
  
  /////////////////////////////////////////////////////////////////
  //                          DOWNSHIFT                          //
  /////////////////////////////////////////////////////////////////
    
  if (showGear>0){          // Valves will not fire past 1ST gear in downshift
    
    if (newState_DOWN == LOW && oldState_DOWN == HIGH) {  //Check to see if button state has changed
      delay(20);                                          // Short delay to debounce button.
      newState_DOWN = digitalRead(BUTTON_DOWN);           // Check if button is still low after debounce.
      if (newState_DOWN == LOW) {
        if (showGear==2){           
          showGear=0;                                     // SKIP neutral
          }                  
        else {
          showGear--;                                     // substracting gear number
       }
       startShift(showGear);

       clutch_servo.write(147);                           // 30 degree servo rotation for clutch cable pull
       
       digitalWrite(val_DOWN, HIGH);
       matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
       matrix.writeDigitRaw(3,B10011100); matrix.writeDisplay();
       delay(120);                                        // time for shift to complete
       digitalWrite(val_DOWN, LOW);

       clutch_servo.write(117);                            // return servo to mid-point, re-engage clutch
       matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
       matrix.writeDigitRaw(3,B00000000); matrix.writeDisplay();
        
       }
      } 
  }

  /////////////////////////////////////////////////////////////////
  //                          NEUTRAL                            //
  /////////////////////////////////////////////////////////////////
    
  if (showGear==0){          // Valves will not fire unless in 1st gear
    
    if (newState_N == LOW && oldState_N == HIGH) {        //Check to see if button state has changed
      delay(20);                                          // Short delay to debounce button.
      newState_N = digitalRead(BUTTON_N);                 // Check if button is still low after debounce.
      if (newState_N == LOW) {     
        showGear++;                                       // adding gear number
       }
       startShift(showGear);

       clutch_servo.write(147);                           // 30 degree servo rotation for clutch cable pull
       
       digitalWrite(val_UP, HIGH);
       delay(25);                                         // time for half shift to complete
                                                          // ADJUST THIS VALUE TO FIND NEUTRAL
       digitalWrite(val_DOWN, HIGH); digitalWrite(val_UP, LOW); 
       delay(10);                                         // time for down valve to open
       digitalWrite(val_DOWN, LOW); // Exhausting valves at neutral
       delay(100);

       clutch_servo.write(117);                            // return servo to mid-point, re-engage clutch
        
       }
  }


  /////////////////////////////////////////////////////////////////
  //                         MPH Display                         //
  /////////////////////////////////////////////////////////////////

//  int potValue = analogRead(pot_pin);                   // Reading potentiometer in order
//  int newpotValue = map(potValue,0,1023,0,255);         // to control DC motor speed
//  analogWrite(PWM_pin, newpotValue);
//  DC_time = read_encoders(encode_pin);
//  DC_MPH = 16.0098/DC_time;
//  Serial.println(DC_MPH);

  
  oldState_UP = newState_UP;                            // Set the last button state to the old state.
  oldState_DOWN = newState_DOWN;                        // Set the last button state to the old state.
  oldState_N = newState_N;                              // Set the last button state to the old state.
}



void startShift(int i) {                                // Keeping track of gear
  switch(i){
    case 0: Serial.print("1st Gear \n");
            matrix.writeDigitNum(4,1); matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
            matrix.writeDisplay();
            break;
    case 1: Serial.print("Neutral \n");
            matrix.writeDigitRaw(4,B11000000);
            matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
            matrix.writeDisplay();
            break;
    case 2: Serial.print("2nd Gear \n");
            matrix.writeDigitNum(4,2); matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
            matrix.writeDisplay();
            break;
    case 3: Serial.print("3rd Gear \n");
            matrix.writeDigitNum(4,3); matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
            matrix.writeDisplay();
            break;
    case 4: Serial.print("4th Gear \n");
            matrix.writeDigitNum(4,4); matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
            matrix.writeDisplay();
            break;
    case 5: Serial.print("5th Gear \n");
            matrix.writeDigitNum(4,5); matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
            matrix.writeDisplay();
            break;
    case 6: Serial.print("6th Gear \n");
            matrix.writeDigitNum(4,6); matrix.writeDigitNum(0,0); matrix.writeDigitNum(1,0);
            matrix.writeDisplay();
            break;
    default:
          matrix.print(0xE, HEX);
          matrix.writeDisplay();
          Serial.print("Error \n");
          break;
  }
}

//
//long read_encoders(int ii) {
//  unsigned long t_pass;
//  int right_value = HIGH;
//  while (digitalRead(ii) == right_value) { }
//  t_pass = millis();
//  while (digitalRead(ii) != right_value) { }
//  t_pass = millis() - t_pass;
//  return t_pass;
//}


