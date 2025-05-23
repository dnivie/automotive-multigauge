#ifndef SENSORREAD_H
#define SENSORREAD_H

#include "Arduino.h"

class Sensor
{
  public:
    float calculateBoost(int16_t boostPressure);
    float calculateAfr(uint16_t afr);
    float calculateTemp(uint16_t temp);
    Sensor();
    float readBoost(void);
    float readAfr(void);
    float readTemp(void);
};

#endif
