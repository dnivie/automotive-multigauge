#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "Kalman.h"
#include "Sensorread.h"
#include "tests.h"

//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // 1.3" screen
//U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 9, 8);    // 2.4" screen
U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI u8g2(U8G2_R0, 13, 51, 10, 9, 8);    // 1.5" screen
// UNO/Mega: scl 13 green, sda 11, res 8 grey, dc 9 purple, cs 10 blue, mosi 51 yellow


uint16_t boostMax;
int16_t boostMin;
uint8_t screenMode = 3; // screen layout (see switch case below)

unsigned long startMillis;  // sensor timer
unsigned long currentMillis;

unsigned long startPeakMillis; // turbo peak value timer
unsigned long currentPeakMillis;

const uint8_t period = 50;  // read sensor interval 50 ms
const uint16_t peakPeriod = 20000; // peakBoost will reset after 20 sec
const uint16_t startUpPeriod = 1500; // startup animation 1.5 sec

const uint8_t sensorHistoryLength = 128;  // horizontal length of screen is 128 pixels
uint8_t sensorHistory[sensorHistoryLength];
uint8_t sensorHistoryPos = sensorHistoryLength - 1;

Kalman kf;
Sensor sensor;
uint8_t noiseCovariance = 60;


void setup(void) 
{
  u8g2.begin();
  delay(1000);
  Serial.begin(9600);
  startMillis = millis(); // start timer
  startPeakMillis = millis();  // timer for peak value
  kf.init(noiseCovariance);
}


void loop(void) 
{  
  float boostPressure;
  float afr;
  currentMillis = millis();
  currentPeakMillis = millis();
  if (currentMillis - startMillis >= period)
  { 
    // read sensors and filter:
    boostPressure = sensor.readBoost();
    boostPressure = kf.filter(boostPressure);

    afr = sensor.readAfr();
    afr = kf.filter(afr);
    
    // Update max and min
    if (boostPressure > boostMax) boostMax = boostPressure;
    if (boostPressure < boostMin) boostMin = boostPressure;

    if (currentPeakMillis - startPeakMillis > peakPeriod) //reset peakValue;
    {
      boostMax = 0;
      startPeakMillis = currentPeakMillis;
    }
    // Log the history:
    addSensorHistory(boostPressure);

    startMillis = currentMillis;
  }

  // display modes:
  switch (screenMode){
    case 0:
      //screen mode 0 (curved line graphics)
      u8g2.firstPage();
      do {
        // Draw current pressure
        u8g2.setFont(u8g2_font_fub20_tf);
        char cstr[6];
        dtostrf((float)boostPressure/1000, 1, 2, cstr);
        u8g2.drawStr(0, 39, cstr);

        // Draw peak pressure
        u8g2.setFont(u8g2_font_fub11_tf);
        dtostrf((float)boostMax / 1000, 1, 2, cstr);
        uint16_t yPos = u8g2.getStrWidth(cstr);
        u8g2.drawStr(128 - yPos, 64, cstr);

        //writing
        u8g2.setFont(u8g2_font_fub11_tf);
        //u8g2.drawStr(103, 64, "bar");
        u8g2.drawStr(98, 14, "AFR");
        u8g2.drawStr(82, 29, "Turbo");
        u8g2.drawStr(64, 64, "bar");

        //Draw afr
        u8g2.setFont(u8g2_font_fub11_tf);
        dtostrf((float)afr, 1, 2, cstr);
        u8g2.drawStr(0, 14, cstr);

        u8g2.setDrawColor(2);
        u8g2.drawBox(0, 15, 128, 1);
        //drawBarGraph(0, 52, 128, 12, boostPressure);
        drawGauge(0, 66, 128, 12, boostPressure);

       } while ( u8g2.nextPage() );
       break;

    case 1:
      //screen mode 1 (horizontal graphics)
      u8g2.firstPage();
      do{
        
        // Draw current pressure
        //u8g2.setFont(u8g2_font_fub20_tf);
        u8g2.setFont(u8g2_font_fub25_tn);
        char cstr[6];
        dtostrf((float)boostPressure/1000, 1, 2, cstr);
        u8g2.drawStr(15, 54, cstr);

        // Draw peak pressure
        u8g2.setFont(u8g2_font_fub11_tf);
        dtostrf((float)boostMax / 1000, 1, 2, cstr);
        uint16_t yPos = u8g2.getStrWidth(cstr);
        u8g2.drawStr(95, 54, cstr);
        
        //writing
        //u8g2.setFont(u8g2_font_7x13B_tf);
        u8g2.setFont(u8g2_font_fub11_tf);

        //Draw afr
        dtostrf((float)afr, 1, 2, cstr);
        u8g2.drawStr(43, 24, cstr);
        
        drawAfrGraphics(0, 10, afr);
        u8g2.drawBox(0, 10, 128, 1);
        //Serial.println(boostPressure);
        u8g2.setDrawColor(2);
        u8g2.drawBox(0, 25, 128, 1);
        drawBarGraph(0, 57, 128, 7, boostPressure);
        //drawGauge(0, 66, 128, 12, boostPressure);

      } while ( u8g2.nextPage() );
      break;

    case 2:
    // screen mode 2 (plotting)
      u8g2.firstPage();
      do {
        // draw current pressure
        u8g2.setFont(u8g2_font_fub20_tf);
        char cstr[6];
        dtostrf((float)boostPressure / 1000, 1, 2, cstr);
        uint16_t yPos = u8g2.getStrWidth(cstr);
        //u8g2.drawStr(65, 32, cstr);
        u8g2.drawStr(128 - yPos, 32, cstr);
        //drawVerticalBar(123, 33, 5, 31, boostPressure);
        
        // draw max pressure
        u8g2.setFont(u8g2_font_mozart_nbp_h_all);
        dtostrf((float)boostMax / 1000, 1, 2, cstr);
        //int yPos = u8g2.getStrWidth(cstr);
        //u8g2.drawStr(128 - yPos, 11, cstr);
        u8g2.drawStr(25, 32, cstr);

        //Draw afr
        //u8g2.setFont(u8g2_font_fub11_tf);
        u8g2.setFont(u8g2_font_mozart_nbp_h_all);
        u8g2.drawStr(0, 20, "boost");
        u8g2.drawStr(0, 32, "max:");
        dtostrf((float)afr, 1, 2, cstr);
        u8g2.drawStr(97, 9, cstr);
        drawAfrGraphics(0, 10, afr);
        //u8g2.drawBox(0, 10, 128, 1);
        drawHorizontalDottedLine(0, 10, 128);

        // plotting
        //u8g2.drawBox(0, 34, 128, 1);
        drawGraph(0, 33, 128, 31);

       } while ( u8g2.nextPage() );
       break;

    case 3:
    // startup screen mode
    u8g2.firstPage();
    do {
        //char cstr[6];
        //u8g2.setFont(u8g2_font_mozart_nbp_h_all);
        //u8g2.drawStr(13, 50, "$ bonsoir, Elliot");
        unittest();

      if (currentMillis - startPeakMillis >= startUpPeriod)
      {
        screenMode = 2;
      }

    } while ( u8g2.nextPage() );
    break;
  }
}


void addSensorHistory(int val) 
{
  sensorHistory[sensorHistoryPos] = val;
  sensorHistoryPos--;
  if (sensorHistoryPos < 0) sensorHistoryPos = sensorHistoryLength - 1;
}


int getSensorHistory(int index) 
{
  index += sensorHistoryPos;
  if (index >= sensorHistoryLength) index = index - sensorHistoryLength;
  return sensorHistory[index];
}

// bar graphics (50% vacuum, 50% positive presure on a horizontal line)
void drawBarGraph(int x, int y, int len, int height, int val) 
{
  uint16_t peakX = 1000;
  if(boostMax > peakX) // if boostPressure exceeds preset value, change graphics to compensate
  {
    peakX = boostMax;
  }
  // Draw the pressure bar behind the graph
  float barLen = (float(val) + peakX) / 1909.0;
  float barLength = barLen * len;

  u8g2.setDrawColor(2);
  u8g2.drawBox(x, y, barLength, height);
  u8g2.drawBox(64, y, 1, 12);
  u8g2.drawBox(x, y-1, 128, 1);
}


void drawVerticalBar(uint8_t x, uint8_t y, uint8_t width, uint8_t maxHeight, uint16_t val)
{
  uint16_t top = 1000;
  float barValue = 0;
  barValue = abs(val);
  
  if(val >= 0)
  {
    barValue = 0;
  }

  //float barHeight = (float(val) + peakX) / 1909.0;
  float barHeight = (barValue / 1000) * maxHeight;
  u8g2.setDrawColor(2);
  u8g2.drawBox(x, y, width, barHeight);
}


void drawAfrGraphics(uint8_t y, uint8_t height, float afr)
{
  //int x = map(afr, 10, 20, 0, 128); 
  float afrNormal = (afr / 10) - 1; // maps afr to [0,1]
  float x = afrNormal * 93; // multiply by horizontal screen length

  u8g2.setDrawColor(2);
  u8g2.drawBox(x-1, y, 1, height);
  u8g2.drawBox(x+1, y, 1, height);
  //u8g2.setFont(u8g2_font_7x13B_tf);
  //u8g2.drawStr(1, 24, "rich");
  //u8g2.drawStr(101, 24, "lean");
}


// draws some fancy line graphics
void drawGauge(uint8_t x, uint8_t y, uint8_t len, uint8_t maxHeight, uint16_t boostPressure) 
{
  uint8_t barLength = (float(boostPressure)/1000) * len;
  uint8_t h = 0;
  uint8_t width = 5;
  for(uint8_t i = 0; i <= barLength; i+=8)
  {
    x = i;
    h = 1+i/8;
    y = (y + h*0.28)-4; 
    u8g2.setDrawColor(3);
    u8g2.drawBox(x,y,width,h);
  }
}

// plotting
void drawGraph(uint8_t x, uint8_t y, uint8_t len, uint8_t height) 
{
  // Draw the lines
  //drawHorizontalDottedLine(x, y, len);
  //drawHorizontalDottedLine(x, y + height, len);
  uint16_t absMin = abs(boostMin);

  if(boostMax < 500)
  {
    boostMax = 500;
  }
  uint16_t range = absMin + boostMax;

  // Draw 0 line
  uint8_t zeroYPos = mapValueToYPos(absMin, range, y, height);
  drawHorizontalDottedLine(x, y, len);

  // Draw the graph line
  for (uint8_t i = 0; i < 128; i++) 
  {
    // Scale the values so that the min is always 0
    uint8_t valueY = getSensorHistory(i) + absMin;
    
    // Calculate the coordinants
    uint8_t yPos = mapValueToYPos(valueY, range, y, height);
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

// Maps a value to a y height
uint8_t mapValueToYPos(uint8_t val, uint8_t range, uint8_t y, uint8_t height) 
{
  float valueY = ((float)val / range) * height;
  return y + height - (int)valueY;
}


void drawHorizontalDottedLine(uint8_t x, uint8_t y, uint8_t len) 
{
  for (uint8_t i = 0; i < len; i++) 
  {
    if (!(i % 4)) u8g2.drawPixel(x + i, y);
  }
}
