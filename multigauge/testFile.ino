/*
#include <AUnit.h>
#include <Arduino.h>
using namespace aunit;

void setup() {
  Serial.begin(9600);
  while (! Serial); // Wait until Serial is ready - Leonardo/Micro

  TestRunner::exclude("*");
  TestRunner::include("looping*");
  TestRunner::include("CustomTestAgain", "example*");
}

void loop() {
  TestRunner::run();
}
*/
// https://github.com/bxparks/AUnit#defining-the-tests