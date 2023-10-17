#ifndef SENSORREAD_H
#define SENSORREAD_H

#include "Arduino.h"

class Sensor
{
  private:
    float calculateBoost(uint16_t boostPressure);
    float calculateAfr(uint16_t afr);
  public:
    Sensor();
    float readBoost(void);
    float readAfr(void);
};

#endif
