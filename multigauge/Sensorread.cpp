#include "Sensorread.h"
#include "Kalman.h"
#define boostPin A0
#define afrPin A1
#define tempPin A2
#define seriesResistor 2800
#define thermistorNominal 2800 // resistance at 20-25C
#define temperatureNominal 20 //eller 25?
#define bCoefficient 3950 // value of the other resistor

Sensor::Sensor() {}

float Sensor::readAfr(void)
{
  float afrData;
  afrData = Sensor::calculateAfr(analogRead(afrPin));
  
  return afrData;
}


float Sensor::readBoost(void)
{
  float boostData, boostPressure;
  boostData = Sensor::calculateBoost(analogRead(boostPin));
  // Sensor should read 0 at athmospheric pressure (tuned with sensor in car):
  boostPressure = boostData - 900;

  return boostPressure;
}


float Sensor::readTemp(void)
{
  float tempData;
  tempData = Sensor::calculateTemp(analogRead(tempPin));
  
  return tempData;
}


float Sensor::calculateAfr(uint16_t n)
{
  // 10-20afr
  return ((n * (5.0/1023.0)) * 2) + 10;
}


float Sensor::calculateTemp(uint16_t t)
{
  float vin = t * (5.0/1023); // convert from 10bit to 5V range
  // skalerer 10bit til en verdi mellom -20 og 130
  // rmin = 0
  // rmax = 1023
  // tmin = -20
  // tmax = 130
  // normalisedValue = (t / 1023) * (130 - (-20)) + (-20)
  // normalisedValue = ((t / 1023) * (150)) -20
  // normalisedValue = t/0.1466 - 20

  /*
  voltage divider:
  V_out = V_in * (R_2 / R_1 + R_2)
  R_1 = sensor
    R_1 = 15462 ohm @ -20
    R_1 = 89 ohm @ 130
  R_2 = 2800 ohm

  convert value to resistance:
  input = (1023 / input) - 1
  thermistor resistance = 2800ohm / input
  https://learn.adafruit.com/thermistor/using-a-thermistor
  */
  // converting to resistance:

  uint8_t therm_res = (1023.0/t) - 1;
  therm_res = seriesResistor / t;

  // converting to temperature:
  float steinhart;
  steinhart = therm_res / thermistorNominal; // R/R_0
  steinhart = log(steinhart); // ln(R/R_0)
  steinhart /= bCoefficient;  // 1/B * ln(R/R_0)
  steinhart += 1.0 / (temperatureNominal + 273.15); // + (1/T_0)
  steinhart = 1.0 / steinhart;  // invert
  steinhart -= 273.15; // convert to Celsius

  return steinhart;
}


float Sensor::calculateBoost(uint16_t m)
{
  /*
  Scale the sensor reading into range
  https://stats.stackexchange.com/a/281164
  m = measurement to be scaled
  rmin = minimum of the range of the measurement
  rmax = maximum of the range of the measurement
  tmin = minimum of the range of the desired target scaling
  tmax = maximum of the range of the desired target scaling

  normalisedValue = ((m − rmin) / (rmax − rmin)) * (tmax − tmin) + tmin
  Sensor voltage ranges from 0.273 to 4.827v, converted to analogRead values (0 min, 1023 max) that's 56 to 988
  
  rmin = 56
  rmax = 988
  
  tmin = 20
  tmax = 2000
  
  normalisedValue = ((m − 56) / (988 − 56)) * (2000 − 20) + (20)
  normalisedValue = ((m − 56) / 932) * 2000
  normalisedValue = (m − 56) / 0.466
  normalisedValue = ((m - 9) / (921 - 9)) * (3000 - 0) - 0
  */
  return (m-56)/0.466; // error of +10
  //return ((495*(m-56))/233)+20
}

