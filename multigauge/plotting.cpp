#include "plotting.h"
#include <U8g2lib.h>
#include <Arduino.h>

uint16_t boostMax = 0;
int16_t boostMin = 0;
const uint8_t sensorHistoryLength = 128;
uint8_t sensorHistory[sensorHistoryLength];
uint8_t sensorHistoryPos = sensorHistoryLength - 1;
U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI u8g2(U8G2_R0, 13, 51, 10, 9, 8);    // 1.5" screen

void Plotting::init()
{
    u8g2.begin();
}

void Plotting::drawHorizontalDotLine(uint8_t x, uint8_t y, uint8_t len) 
{
    for (uint8_t i = 0; i < len; i++) 
    {
        if (!(i % 4)) u8g2.drawPixel(x + i, y);
    }
}

void Plotting::drawGraph(uint8_t x, uint8_t y, uint8_t len, uint8_t height)
{
    uint16_t absMin = abs(boostMin);    
    if(boostMax < 500)
    {
        boostMax = 500;
    }
    uint16_t range = absMin + boostMax; 
    // Draw 0 line
    //uint8_t zeroYPos = Plotting::mapValToYPos(absMin, range, y, height);
    Plotting::drawHorizontalDotLine(x, y, len);    
    // Draw the graph line
    for (uint8_t i = 0; i < 128; i++) 
    {
        // Scale the values so that the min is always 0
        uint8_t valueY = Plotting::getSensorHistory(i) + absMin;
    
        // Calculate the coordinants
        uint8_t yPos = Plotting::mapValToYpos(valueY, range, y, height);
        uint8_t xPos = len - i;
        u8g2.drawPixel(xPos, yPos);
        u8g2.drawPixel(xPos, yPos+1); 
        /*
        if (yPos < zeroYPos) 
        {
            // Point is above zero line, fill in space under graph
            u8g2.drawVLine(xPos, yPos, zeroYPos + 1 - yPos);
            //u8g2.drawPixel(xPos, yPos);
        } else 
        {
        // Point is below zero line, draw graph line without filling in
            u8g2.drawPixel(xPos, yPos);
        }
        */
    }
}

uint8_t Plotting::mapValToYpos(uint8_t val, uint8_t range, uint8_t y, uint8_t height)
{
    float valueY = ((float)val / range) * height;
    return y + height - (int)valueY;
}

int16_t Plotting::getSensorHistory(uint8_t index)
{
    index += sensorHistoryPos;
    if (index >= sensorHistoryLength) index = index - sensorHistoryLength;
    return sensorHistory[index];
}

void Plotting::addSensorHistory(int16_t val)
{
    sensorHistory[sensorHistoryPos] = val;
    sensorHistoryPos--;
    if (sensorHistoryPos < 0) sensorHistoryPos = sensorHistoryLength - 1;

    if (val > boostMax) boostMax = val;
    if (val < boostMin) boostMin = val;
}


uint16_t Plotting::getBoostMax()
{
    return boostMax;
}

int16_t Plotting::getBoostMin()
{
    return boostMin;
}

void Plotting::resetBoostMax()
{
    boostMax = 0;
}
