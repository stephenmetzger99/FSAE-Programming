#include <Canbus.h>
char UserInput;
int data;
char buffer[456];  //Data will be temporarily stored to this buffer before being written to the file


void setup(){
Serial.begin(9600);
Serial.println("CAN-Bus Demo");

if(Canbus.init(CANSPEED_500))  /* Initialise MCP2515 CAN controller at the specified speed */
  {
    Serial.println("CAN Init ok");
  } else
  {
    Serial.println("Can't init CAN");
  } 
   
  delay(1000); 

Serial.println("Please choose a menu option.");

}




void loop(){
 Canbus.ecu_req(ENGINE_RPM, buffer);
 Serial.print("Engine RPM: ");
 Serial.println(buffer);
}

