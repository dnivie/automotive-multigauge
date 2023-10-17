#ifndef TESTS_H
#define TESTS_H
#include "Arduino.h"
#include <ArduinoUnit.h>
//#include <AUnit.h>
/*
test(calculateAfr) {
  uint8_t analogReading = 500;
  assertTrue(x, 1);
}

test(readAfr) {
  uint8_t analogReading = 500;
  assertTrue(x, 1);
}

test(calculateBoost) {
  uint8_t analogReading = 500;
  asserTrue(x, 1);
}

test(readBoost) {
  uint8_t analogReading = 500;
  asserTrue(x, 1);
}
*/
test(kalmanfilter) { 
  pass(); 
}

void unittest() {
  Test::run();
}

// https://github.com/bxparks/AUnit#defining-the-tests
// https://github.com/mmurdoch/arduinounit/blob/master/guidebook.md

#endif
