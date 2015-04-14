#include "Arduino.h"

// default command to reset
#define SR_RESET_CMD "AT+RESET!"

// default command to support reset
#define SR_RESET_SUPPORT_CMD "AT+RESET?"

// default response to support reset
#define SR_RESET_SUPPORT_RESPONSE "YES"

// uncomment to get additional logging
// #define SR_DEBUG 1

// uncomment to set LED on ppin 13 to HIGH for 2 seconds on reset
// #define SR_LED_ON_RESET 1

const char *sr_reset_cmd;           // command to perform reset
const char *sr_reset_support_cmd;   // command to check if serial reset is supported
const char *sr_reset_support_response; // reply to send for reset supported?

int sr_reset_index = 0;
int sr_reset_support_index = 0;

int sr_available = 0;       // chars available
int sr_byte;                // income buffer as 1 char
int *sr_bytes = &sr_byte;  // income buffer
boolean delete_sr_bytes = false; // indicates new array was allocated and need to delete[] later
int sr_reset_pin;           // pin to set LOW for reset (wired with arduino reset pin)
void (*sr_reset_func)();    // function pointer to perform reset actually

// need to jump to bootloader, waiting upload commands code
#if defined(__AVR_ATmega328__)
    // Arduino Pro Mini
    #define SR_BOOTLOADER_JUMP "jmp 0x1f00" // bootloader start address
#elif defined(__AVR_ATmega328P__)
    // Arduino Duemilanove, Uno, Nano
    #define SR_BOOTLOADER_JUMP "jmp 0x7e0a" // bootloader after if address (not start address)
#elif defined(__AVR_ATmega2560__)
    // Arduino Mega 2560
    #define SR_BOOTLOADER_JUMP "jmp 0xfc00" // bootloader start address
#elif defined(__AVR_ATmega32U4__)
    // Arduino Leonardo
    #define SR_BOOTLOADER_JUMP "jmp 0x7e0a" // bootloader start address
#endif

void sr_perform_reset_jump_bootloader() {
  #ifdef SR_DEBUG
  Serial.println("reset using jump to bootloader");
  #endif
  
  // https://arduino.stackexchange.com/questions/9926/uploading-over-bluetooth-low-energy-bootloader-not-responding
  asm volatile (SR_BOOTLOADER_JUMP);
}

void sr_send_support_response() {
  Serial.print(sr_reset_support_response);
}

void sr_perform_reset_pin() {
  #ifdef SR_DEBUG
  Serial.println("reset using write #sr_reset_pin to LOW");
  #endif
  digitalWrite(sr_reset_pin, LOW);
}

void sr_setup(unsigned long baud, const char *reset_cmd, const char *reset_support_cmd, const char *reset_support_response, void (*reset_func)()) {
    Serial.begin(baud);
    
    sr_reset_cmd = reset_cmd;
    sr_reset_support_cmd = reset_support_cmd;
    sr_reset_support_response = reset_support_response;
    sr_reset_func = reset_func;

    #ifdef SR_LED_ON_RESET
    pinMode(13, OUTPUT);
    #endif
}

void sr_setup(unsigned long baud, const char *reset_cmd, const char *reset_support_cmd, const char *reset_support_response) {
    sr_setup(baud, reset_cmd, reset_support_cmd, reset_support_response, &sr_perform_reset_jump_bootloader);
}

void sr_setup(unsigned long baud) {
  sr_setup(baud, SR_RESET_CMD, SR_RESET_SUPPORT_CMD, SR_RESET_SUPPORT_RESPONSE);
}

void sr_setup(unsigned long baud, const char *reset_cmd, const char *reset_support_cmd, const char *reset_support_response, int reset_pin) {
  sr_reset_pin = reset_pin;

  // init reset pin
  digitalWrite(sr_reset_pin, HIGH);
  pinMode(sr_reset_pin, OUTPUT);

  sr_setup(baud, reset_cmd, reset_support_cmd, reset_support_response, &sr_perform_reset_pin);
}

// setup with custom commands, "AT+RESET?" as support command and "YES" as response. used pin wired to reset 
void sr_setup(unsigned long baud, int reset_pin) {
  sr_setup(baud, SR_RESET_CMD, SR_RESET_SUPPORT_CMD, SR_RESET_SUPPORT_RESPONSE, reset_pin);
}

int Serial_available() {
    return sr_available;
}

int Serial_read() {
    if (sr_available < 0)
        return -1;
  
    int inByte = sr_bytes[--sr_available];
    if (sr_available == 0 && delete_sr_bytes) {
        // restore buffer as 1 char buffer
        delete[] sr_bytes;
        sr_bytes = &sr_byte;
        delete_sr_bytes = false;
    }
    return inByte;
}

void serial_check(int in_byte, const char *cmd, int *index, int *index2, void (*cmd_func)()) {
    
    // if it's handled by another command
    if (*index == -1) {     
        #ifdef SR_DEBUG
        Serial.println("index = -1, set index = 0 and skipping");
        #endif
      
        *index = 0;
        return;
    }
  
    if (in_byte == cmd[*index]) {
        // entered char is cmd character
 
       (*index) = (*index) + 1;
        if (cmd[*index] == 0) {
          
            // 13 is HIGH for 2 seconds to indicate reset is done 
            #ifdef SR_LED_ON_RESET
            digitalWrite(13, HIGH);
            delay(1000);
            digitalWrite(13, LOW);
            #endif
          
            (*index) = 0;
            (*index2) = -1;
            
            cmd_func(); // perform reset or send reset is supported
        }
      
        #ifdef SR_DEBUG  
        Serial.write("index = ");
        Serial.print((byte)(*index));
        Serial.write('\n');
        #endif 
     
    } else {
        // entered char is NOT cmd character
        
        #ifdef SR_DEBUG  
        Serial.println("unknown char");
        #endif
      
        if (*index > 0) {
          
            // can't send if it's suitable for another command for now
            if (*index2 == 0) {
          
              // got character not from cmd, so we should to return characters
              // starting sr_reset_cmd until current index and then this character
            
              sr_available = (*index) + 1;
              (*index) = 0;
            
              #ifdef SR_DEBUG
              Serial.print("Copy ");
              Serial.print((byte)(sr_available - 1));
              Serial.println(" bytes");
              #endif
            
              // recreate input buffer using cmd characters
              sr_bytes = new int[sr_available + 1];
              delete_sr_bytes = true;
              sr_bytes[0] = in_byte;
              for (int i=0; i<(sr_available-1); i++) {
                  sr_bytes[i + 1] = cmd[sr_available - i - 2];
              }
              sr_bytes[sr_available] = 0; // null-terminated array
            
              #ifdef SR_DEBUG
              Serial.print("input buffer now contains ");
              Serial.print(sr_available);
              Serial.println(" bytes");
              #endif
            } else {
              (*index) = 0;
            }
            
        } else {
          
            if ((*index2) == 0) {
                #ifdef SR_DEBUG
                Serial.println("index = 0");
                #endif
      
                // pass in_byte next ...
                sr_available++;
                sr_byte = in_byte;
           }
        }
    }
}
 
void serialEvent() {
    if (Serial.available() <= 0)
        return;
        
    int in_byte = Serial.read();
    
    #ifdef SR_DEBUG
    Serial.print("\n\n");
    Serial.println("check reset support cmd");
    #endif
    serial_check(in_byte, sr_reset_support_cmd, &sr_reset_support_index, &sr_reset_index, &sr_send_support_response); // check reset supported
    
    #ifdef SR_DEBUG
    Serial.println("check reset cmd");
    #endif
    serial_check(in_byte, sr_reset_cmd, &sr_reset_index, &sr_reset_support_index, sr_reset_func);                     // check reset
} 