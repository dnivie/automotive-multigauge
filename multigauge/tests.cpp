/*

#line 2 "testfile.ino"
//#include <AUnit.h>
#include <Arduino.h>
#include <ArduinoUnit.h>

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

test(kalmanfilter) { 
  pass(); 
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println(F("This test should produce the following:"));
  Serial.println(F("3 passed, 0 failed, 0 skipped, 0 timed out, out of 3 test(s)."));
  Serial.println(F("----"));
}

void loop() {
  Test::run();
}
*/
// https://github.com/bxparks/AUnit#defining-the-tests
// https://github.com/mmurdoch/arduinounit/blob/master/guidebook.md