#include <SerialReset.h>

/*

  http://playground.arduino.cc/Main/ArduinoReset
  
*/

void setup() {
  sr_setup(115200); // Setup SerialReset
  
  #if defined(__AVR_ATmega32U4__)
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  #endif
  
  Serial.println("ready");
}

void loop() {
  // use Serial_available() instead of Serial.available() 
  while (Serial_available() > 0) {

    // use Serial_read() instead Serial.read()
    int inByte = Serial_read();    
    Serial.write(inByte); // echo
  }
}
