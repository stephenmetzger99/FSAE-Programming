#include "Canbus.h"

void setup() {
  Serial.begin(9600); // For debug use
  while(!Canbus.init(CANSPEED_500))  //Initialise MCP2515 CAN controller at the specified speed
  {
    if (Canbus.init(CANSPEED_500)) {
      break;
    }
  }
}


char buffer[64];
char* rpm_rawdata;
uint16_t rpm;

void loop() {
  

}
