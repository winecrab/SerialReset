#include <SerialReset.h>

/*

  Echo sketch with serial reset using bootloader jump
  Open serial monitor at 115200, send "AT+RESET?" to check serial reset support, "AT+RESET!" to reset the board

  http://playground.arduino.cc/Main/ArduinoReset

*/

void setup() {
  Serial_begin(SERIAL_RESET_BAUD_RATE); // use Serial_begin() instead of Serial.begin()
  
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
