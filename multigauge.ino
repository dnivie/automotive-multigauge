#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

//https://github.com/olikraus/u8g2

//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2); //type of  screen
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

float boostPressure;
float oilPressure;
float afrNumber;
int boostMax = 0;
int boostMin = 0;

unsigned long startMillis;  //timer
unsigned long currentMillis;

unsigned long startPeakMillis; //peak timer
unsigned long currentPeakMillis;

const unsigned long period = 50;  //read sensor interval
const unsigned long peakPeriod = 8000; //peakBoost will reset after 8sec

const int sensorHistoryLength = 128;  //length of screen is 128 pixels
int sensorHistory[sensorHistoryLength];
int sensorHistoryPos = sensorHistoryLength - 1;


void setup(void) {
  u8g2.begin();
  startMillis = millis(); //start timer
  startPeakMillis = millis();  //timer for peakValue
  Serial.begin(9600);
}


void loop(void) {
  //Only read from the sensors every 50 ms.

  currentMillis = millis();
  currentPeakMillis = millis();
  if (currentMillis - startMillis >= period) {
    readSensorData();     //read from boostSensor
    readOilSensorData();  //oilPressure
    readAfrSensor();      //Air to fuel ratio
    startMillis = currentMillis;

    //serial monitor output
    Serial.print("boost: ");
    Serial.print(boostPressure/1000);
    Serial.print(" bar, ");
    Serial.print("afr: ");
    Serial.print(afrNumber);
    Serial.print(", oilP: ");
    Serial.print(oilPressure/100, 2);
    Serial.println(" bar ");
  }

  u8g2.firstPage();
  do {
    // Draw current pressure
    u8g2.setFont(u8g2_font_fub20_tf);
    char cstr[6];
    dtostrf((float)boostPressure/1000, 1, 2, cstr);
    u8g2.drawStr(0, 20, cstr);

    // Draw peak pressure
    u8g2.setFont(u8g2_font_fub11_tf);
    dtostrf((float)boostMax / 1000, 1, 2, cstr);
    int yPos = u8g2.getStrWidth(cstr);
    u8g2.drawStr(128 - yPos, 11, cstr);

    //writing
    u8g2.setFont(u8g2_font_fub11_tf);
    u8g2.drawStr(82, 23, "Boost");
    u8g2.drawStr(82, 60, "Oil P.");
    u8g2.drawStr(82, 40, "AFR");

    //Draw oil pressure
    u8g2.setFont(u8g2_font_fub14_tf);
    dtostrf((float)oilPressure / 100, 1, 2, cstr);
    u8g2.drawStr(0, 60, cstr);

    //Low oil pressure warning
    if (oilPressure < 60){ //warning set to 0.6bar
      u8g2.setFont(u8g2_font_fub11_tf);
      u8g2.drawStr(82, 60, "LOW!");
    }

    //Draw afr
    u8g2.setFont(u8g2_font_fub14_tf);
    dtostrf((float)afrNumber, 1, 2, cstr);
    u8g2.drawStr(0, 40, cstr);

    //Serial.println(currentMillis);


    //drawBarGraph(0, 22, 128, 8);
    //drawGraph(0, 32, 128, 31);

  } while ( u8g2.nextPage() );
}


float normaliseSensorData(int m) {  //calculate sensorValue
  /*
    Scale the sensor reading into range
    m = measurement to be scaled
    rmin = minimum of the range of the measurement
    rmax = maximum of the range of the measurement
    tmin = minimum of the range of the desired target scaling
    tmax = maximum of the range of the desired target scaling
    normalisedValue = ((m − rmin) / (rmax − rmin)) * (tmax − tmin) + tmin
    https://stats.stackexchange.com/a/281164
  */

  /*
    //from github
    Sensor voltage ranges from 0.5v to 4.5v, converted to analogRead values (0 min, 1023 max) that's 102 to 921
    rmin = 102
    rmax = 921
    Sensor reads from 0 to 50psi
    tmin = 0
    tmax = 5000
    normalisedValue = ((m − 102) / (921 − 102)) * (5000 − 0) + 0
    normalisedValue = ((m − 102) / 819) * 5000
    normalisedValue = (m − 102) / 0.1638

    //my sensor (psi)
    Sensor voltage ranges from 0.045v to 5v, converted to analogRead values (0 min, 1023 max) that's 9 to 1023
    rmin = 9
    rmax = 1023
    Sensor reads from -14.5 to 29psi
    tmin = 0
    tmax = 2900
    normalisedValue = ((m − 9) / (1023 − 9)) * (2900 − 0) + 0
    normalisedValue = ((m − 9) / 1012) * 2900
    normalisedValue = (m − 9) / 0.1638

    //my sensor (bar)
    0.045 to 5v
    rmin = 9
    rmax = 1023
    Sensor reads from -1 to 2 bar
    tmin = 0 (-100?)
    tmax = 300
    normalisedValue = ((m - 9) / (1023 - 9)) * (4000 - 0) - 0
  */



  return (m-9)/0.253;
}

float calculateOilData(int o){  //calculate oilPressure function
  //0-150psi

  /*
  0-10bar
  rmin = 102 (0.5v)
  rmax = 921 (4.5v)
  tmin = 0
  tmax = 100
  normalisedValueBAR = ((o - 102) / (921 - 102)) * (100 - 0) + 0
  //(o - 102)

  */
  //return (o - 102) / 0.0546;
  return (o-102)/0.819;
}

float calculateAfrData(int n){  //bosch afr wideband sensor
  //10-20afr
  return ((n * (5.0/1023.0)) * 2) + 10;
}

void readAfrSensor(void){
  float afrData = calculateAfrData(analogRead(A2));
  afrNumber = afrData;
}


void readOilSensorData(void){
  float absoluteOilPressure = calculateOilData(analogRead(A1));
  oilPressure = absoluteOilPressure-124;
  if (oilPressure < 0) oilPressure = 0; //oilPressure can't be negative

}


void readSensorData(void) {
  float absolutePressure = normaliseSensorData(analogRead(A0));

  // Subtract 14.7 psi == pressure at sea level (1 bar)

  boostPressure = absolutePressure - 420;


  // Update max and min
  if (boostPressure > boostMax) boostMax = boostPressure;
  if (boostPressure < boostMin) boostMin = boostPressure;

  if (currentPeakMillis - startPeakMillis > peakPeriod){ //reset peakValue;
      boostMax = 0;
      startPeakMillis = currentPeakMillis;
    }

  // Log the history
  addSensorHistory(boostPressure);
}


void addSensorHistory(int val) {
  sensorHistory[sensorHistoryPos] = val;
  sensorHistoryPos--;
  if (sensorHistoryPos < 0) sensorHistoryPos = sensorHistoryLength - 1;
}


int getSensorHistory(int index) {
  index += sensorHistoryPos;
  if (index >= sensorHistoryLength) index = index - sensorHistoryLength;
  return sensorHistory[index];
}


//Display functions below are currently replaced by afr and oilpressure.

/*
void drawGraph(int x, int y, int len, int height) {
  // Draw the lines
  drawHorizontalDottedLine(x, y, len);
  drawHorizontalDottedLine(x, y + height, len);

  var absMin = Math.abs(boostMin);
  int absMin = abs(boostMin);
  int range = absMin + boostMax;

  // Draw 0 line
  int zeroYPos = mapValueToYPos(absMin, range, y, height);
  drawHorizontalDottedLine(x, zeroYPos, len);

  // Draw the graph line
  for (int i = 0; i < 128; i++) {
    // Scale the values so that the min is always 0
    int valueY = getSensorHistory(i) + absMin;

    // Calculate the coordinants
    int yPos = mapValueToYPos(valueY, range, y, height);
    int xPos = len - i;
    if (yPos < zeroYPos) {
      // Point is above zero line, fill in space under graph
      u8g2.drawVLine(xPos, yPos, zeroYPos + 1 - yPos);
    } else {
      // Point is below zero line, draw graph line without filling in
      u8g2.drawPixel(xPos, yPos);
    }
  }


}


void drawBarGraph(int x, int y, int len, int height) {
  if (boostPressure > 0) {
    // Draw the pressure bar behind the graph
    int barLength = ((float)boostPressure / boostMax) * len;
    u8g2.setDrawColor(2);
    u8g2.drawBox(x, y, barLength, height);
    u8g2.setDrawColor(1);
  }
}


// Maps a value to a y height
int mapValueToYPos(int val, int range, int y, int height) {
  float valueY = ((float)val / range) * height;
  return y + height - (int)valueY;
}



void drawHorizontalDottedLine(int x, int y, int len) {
  for (int i = 0; i < len; i++) {
    if (!(i % 4)) u8g2.drawPixel(x + i, y);
  }
}
*/
