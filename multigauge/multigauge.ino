#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "Kalman.h"
#include "Sensorread.h"
//#include "tests.h"
#include "Graphics.h"


unsigned long startMillis;  // sensor read timer
unsigned long currentMillis;

unsigned int startPeakMillis; // turbo peak value timer
unsigned int currentPeakMillis;

uint8_t screenMode = 0; // screen layout (see switch case below)
const uint8_t period = 50;  // read sensor interval in ms
const uint16_t peakPeriod = 20000; // peakBoost will reset after 20 sec
const uint16_t startUpPeriod = 1500; // startup screen duration 1.5 sec
uint8_t noiseCovariance = 60; // Kalman filtering


Kalman kf;
Sensor sensor;
Graphics screen;


void setup(void) 
{
  delay(1000);
  Serial.begin(9600);
  startMillis = millis(); // start timer
  startPeakMillis = millis();  // timer for peak value
  kf.init(noiseCovariance);
  screen.init();
}


void loop(void) 
{  
  float boostPressure;
  float afr;
  int8_t waterTemp;
  currentMillis = millis();
  currentPeakMillis = millis();

  if (currentMillis - startMillis >= period)
  { 
    // read sensors and filter:
    boostPressure = sensor.readBoost();
    boostPressure = kf.filter(boostPressure);

    afr = sensor.readAfr();
    afr = kf.filter(afr);

    waterTemp = sensor.readTemp();
    waterTemp = kf.filter(waterTemp);
    
    // Update max and min
    //if (boostPressure > boostMax) boostMax = boostPressure;
    //if (boostPressure < boostMin) boostMin = boostPressure;

    if (currentPeakMillis - startPeakMillis > peakPeriod) // reset max boost pressure;
    {
      screen.resetBoostMax();
      startPeakMillis = currentPeakMillis;
    }

    screen.addSensorHistory(boostPressure);
    startMillis = currentMillis;
  }

  // display modes:
  switch (screenMode)
  {
    case 0:
      // startup screen
      screen.screenMode0();
      //unittest();

      if (currentMillis - startPeakMillis >= startUpPeriod)
      {
        screenMode = 3; // switch screen mode
      }
      
      break;

    case 1:
      screen.screenMode1(boostPressure, afr);
      
      break;

    case 2:
      screen.screenMode2(boostPressure, afr);
    
      break;

    case 3:
      screen.screenMode3(boostPressure, afr);
      break;

    case 4:
      screen.screenMode4(boostPressure, afr, waterTemp);
      break;
    
  }
}
