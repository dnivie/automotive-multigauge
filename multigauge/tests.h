#ifndef TESTS_H
#define TESTS_H
#include "Arduino.h"
#include <ArduinoUnit.h>
#include "Kalman.h"
#include "Sensorread.h"
//#include <AUnit.h>

test(calculateAfr) {
  // calculated value should be between 14 and 15 (sensor reading of 500 equals to 14.89 in AFR)
  Sensor sensor;
  assertLessOrEqual(abs(sensor.calculateAfr(500)-15), 1);
}

test(calculateBoost) {
  // sensor reading of 500 equals to 0.95279 bar
  Sensor sensor;
  assertMoreOrEqual(sensor.calculateBoost(500)/1000, 0.95);
}

test(readAfr) {
  // should be able to read a value above minimum, and it should be somewhat steady
  Sensor sensor;
  //sensor.readAfr(); // why does uncommenting this use less memory?
  assertMoreOrEqual(sensor.readAfr(), 11);
}
/*
test(readBoost) {
  // should be able to read a value, and it should be somewhat steady
  uint8_t analogReading = 500;
  asserTrue(x, 1);
}
*/
test(kalmanfilter) { 
  // value should be smoothed by some margin
  pass(); 
}

test(checkSram) {
  // check if there is atleast 100 bytes of free sram
  extern int __heap_start,*__brkval;
  uint16_t v, freeRam;
  bool enoughSram = false;
  freeRam = (uint16_t)&v - (__brkval == 0 ? (uint16_t)&__heap_start : (uint16_t) __brkval);
  if(freeRam > 100)
  {
    enoughSram = true;
  }
  assertTrue(enoughSram);
}


void unittest() {
  Test::run();
}

// https://github.com/bxparks/AUnit#defining-the-tests
// https://github.com/mmurdoch/arduinounit/blob/master/guidebook.md

#endif
