#include <Wire.h>
#include "PCF8574.h"
#include "Bounce2pcf.h"

PCF8574 IN_1(0x20);

#define DEBOUNCE_TIME 5 // debounceTime

Bounce2pcf * buttons = new Bounce2pcf[8]; 

void setup() {
  for (uint8_t i = 0; i < 8; i++) {
    buttons[i].attach(&IN_1, i);
    buttons[i].interval(DEBOUNCE_TIME); 
  }
}

void loop() {
  for (int i = 0; i < 8; i++) {
    if (buttons[i].update() && buttons[i].fell()) {
      Serial.printf("Button %d pressed!, i");
    }
  }
}
