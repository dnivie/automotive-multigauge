#ifndef SENSORREAD_H
#define SENSORREAD_H

#include "Arduino.h"

class Sensor
{
  private:
    float calculateBoost(int boostPressure);
    float calculateAfr(int afr);
  public:
    Sensor();
    float readBoost(void);
    float readAfr(void);
};

#endif