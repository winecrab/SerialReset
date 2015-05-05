/*
   Anton Smirnov, 2015
   APPLoader project, SerialReset library

   http://playground.arduino.cc/Main/ArduinoReset
   http://weblog.jos.ph/development/arduino-reset-hack/
*/

#ifndef __SERIAL_RESET_H__
#define __SERIAL_RESET_H__

#if defined(ARDUINO_AVR_DUEMILANOVE) || defined(ARDUINO_AVR_NANO)
   #define SERIAL_RESET_BAUD_RATE 57600 // for Duemilanove and Nano
#else
	#define SERIAL_RESET_BAUD_RATE 115200
#endif

// #warning "Serial baud rate is set to " SERIAL_RESET_BAUD_RATE
#pragma message "Serial baud rate is set to " SERIAL_RESET_BAUD_RATE

// setup with "AT+RESET!" as command, "AT+RESET?" as support command and "YES" as response. bootloader jump used for reset
void Serial_begin(unsigned long baud);

// setup with custom commands, bootloader jump used for reset
void Serial_begin(unsigned long baud, const char *reset_cmd, const char *reset_support_cmd, const char *reset_support_response);

// setup with custom commands, "AT+RESET?" as support command and "YES" as response. used pin wired to reset 
void Serial_begin(unsigned long baud, int reset_pin);

// setup with custom commands, used pin wired to reset 
void Serial_begin(unsigned long baud, const char *reset_cmd, const char *reset_support_cmd, const char *reset_support_response, int reset_pin);

// use instead of Serial.available() 
int Serial_available();

// use instead of Serial.read();
int Serial_read();

#endif