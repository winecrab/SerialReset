#include <SerialReset.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald

  modified 13 Apr 2015
  by Anton Smirnov
  

  http://playground.arduino.cc/Main/ArduinoReset
  
 */

const int LED = 13;
const int INTERVAL = 1000;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial_begin(SERIAL_RESET_BAUD_RATE); // use Serial_begin() instead of Serial.begin()

  #if defined(__AVR_ATmega32U4__)
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  #endif

  // initialize digital pin 13 as an output.
  pinMode(LED, OUTPUT);
}

unsigned long time;
int level = LOW;

// the loop function runs over and over again forever
void loop() {
  /*
  Since serialEvent() is invoked between loop invocations:
  1. don’t use delay() with large interval value
  or
  2. run serialResetEvent() within loop() to read serial data and handle reset commands
  */

  unsigned long curTime = millis();
  if (curTime - time > INTERVAL) {
    level = 1 - level;
    time = curTime;
    digitalWrite(LED, level);
  }
}