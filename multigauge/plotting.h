#ifndef PLOTTING_H
#define PLOTTING_H

#include "Arduino.h"

class Plotting
{
    public:
        uint8_t mapValToYpos(uint8_t val, uint8_t range, uint8_t y, uint8_t height);
        int16_t getSensorHistory(uint8_t index);
        void addSensorHistory(int16_t val);
        void init();

        //const uint8_t sensorHistoryLength = 128;
        //uint8_t sensorHistory[sensorHistoryLength];
        //uint8_t sensorHistoryPos = sensorHistoryLength - 1;
    
        void drawGraph(uint8_t x, uint8_t y, uint8_t len, uint8_t height);
        void drawHorizontalDotLine(uint8_t x, uint8_t y, uint8_t len);
        uint16_t getBoostMax();
        int16_t getBoostMin();
        void resetBoostMax();
};

#endif
