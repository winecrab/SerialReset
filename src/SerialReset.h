/*
   Anton Smirnov, 2015
   APPLoader project, SerialReset library

   http://playground.arduino.cc/Main/ArduinoReset
*/

#ifndef __SERIAL_RESET_H__
#define __SERIAL_RESET_H__

// setup with "AT+RESET!" as command, "AT+RESET?" as support command and "YES" as response. bootloader jump used for reset
void sr_setup(unsigned long baud);

// setup with custom commands, bootloader jump used for reset
void sr_setup(unsigned long baud, const char *reset_cmd, const char *reset_support_cmd, const char *reset_support_response);

// setup with custom commands, "AT+RESET?" as support command and "YES" as response. used pin wired to reset 
void sr_setup(unsigned long baud, int reset_pin);

// setup with custom commands, used pin wired to reset 
void sr_setup(unsigned long baud, const char *reset_cmd, const char *reset_support_cmd, const char *reset_support_response, int reset_pin);

// use instead of Serial.available() 
int Serial_available();

// use instead of Serial.read();
int Serial_read();

#endif