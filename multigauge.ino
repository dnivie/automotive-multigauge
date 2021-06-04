#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2); //type of  screen
//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 9, 8);
// UNO/Mega: scl 13, sda 11, res 8, dc 9, cs 10

int boostMax;
int boostMin;
int sensorState = 0;

unsigned long startMillis;  //timer
unsigned long currentMillis;

unsigned long startPeakMillis; //peak timer
unsigned long currentPeakMillis;

const unsigned long period = 50;  // read sensor interval 50 ms
const unsigned long peakPeriod = 15000; // peakBoost will reset after 15 sec.

//const int sensorHistoryLength = 128;  //length of screen is 128 pixels
//int sensorHistory[sensorHistoryLength];
//int sensorHistoryPos = sensorHistoryLength - 1;


void setup(void) {
  int sensors[1] = {analogRead(A1)}; // boost on A0, afr on A1
  // startup sound (Mario power UP)
  /*
  tone(6,1319,125); //e6
  delay(130);
  tone(6,1568,125); //g6
  delay(130);
  tone(6,2637,125); //e7
  delay(130);
  tone(6,2093,125); //c7
  delay(130);
  tone(6,2349,125); //d7
  delay(130);
  tone(6,3136,125); //g7
  delay(125);
  noTone(6);*/

  u8g2.begin();
  startMillis = millis(); //start timer
  startPeakMillis = millis();  //timer for peakValue
  //Serial.begin(9600);
  
  // check if afr logging is connected:
  if (sensors[0] > 0){
    sensorState = 0; // boost and afr
  }
  else{
    sensorState = 1; // boost only
  }
}


void loop(void) {
  
  float boostPressure = 0;
  float afrNumber = 0;
  currentMillis = millis();
  currentPeakMillis = millis();
  if (currentMillis - startMillis >= period) {  // read sensors
    boostPressure = readBoostData();
    afrNumber = readAfrSensor();
    startMillis = currentMillis;
  }

  //the display has 3 modes:
  switch (sensorState){
    case 0:
      //screen mode 0 (all info: Boost pressure and Air Fuel Ratio)
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
        int yPos = u8g2.getStrWidth(cstr);
        u8g2.drawStr(128 - yPos, 64, cstr);

        //writing
        u8g2.setFont(u8g2_font_fub11_tf);
        //u8g2.drawStr(103, 64, "bar");
        u8g2.drawStr(98, 14, "AFR");
        u8g2.drawStr(82, 29, "Turbo");
        u8g2.drawStr(64, 64, "bar");

        //Draw afr
        u8g2.setFont(u8g2_font_fub11_tf);
        dtostrf((float)afrNumber, 1, 2, cstr);
        u8g2.drawStr(0, 14, cstr);

        u8g2.setDrawColor(2);
        u8g2.drawBox(0, 15, 128, 1);
        //drawBarGraph(0, 52, 128, 12, boostPressure);
        drawGauge(0, 66, 128, 12, boostPressure);

       } while ( u8g2.nextPage() );
       break;

    case 1:
      //screen mode 1 (boost pressure only)
      u8g2.firstPage();
      do{
        
        u8g2.setFont(u8g2_font_fub20_tf);
        char cstr[6];
        dtostrf((float)boostPressure/1000, 1, 2, cstr);
        u8g2.drawStr(0, 37, cstr);

        // Draw peak pressure
        u8g2.setFont(u8g2_font_fub11_tf);
        dtostrf((float)boostMax / 1000, 1, 2, cstr);
        int yPos = u8g2.getStrWidth(cstr);
        u8g2.drawStr(128 - yPos, 64, cstr);

        //writing
        u8g2.setFont(u8g2_font_fub11_tf);
        u8g2.drawStr(0,12, "Boost");
        u8g2.drawStr(64,30, "bar");
        
        drawGauge(0, 66, 128, 12, boostPressure);

      } while ( u8g2.nextPage() );
      break;

    case 2:
    // AFR only
      u8g2.firstPage();
      do {
        draw();
        char cstr[6];
        //writing
        u8g2.setFont(u8g2_font_fub11_tf);
        u8g2.drawStr(82, 60, "AFR");

        //Draw afr
        u8g2.setFont(u8g2_font_fub17_tf);
        dtostrf((float)afrNumber, 1, 2, cstr);
        u8g2.drawStr(26, 56, cstr);

       } while ( u8g2.nextPage() );
       break;
  }
}

/*
void draw()
{
  u8g2.drawBitmap(0, 0, 16, 64, boss_graphics);
}*/


float normaliseSensorData(int m) {  //calculate sensorValue
  //check: input voltage from arduino, raw value (engine off)
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
    Sensor voltage ranges from 0.5v to 4.5v, converted to analogRead values (0 min, 1023 max) that's 102 to 921
    rmin = 102
    rmax = 921
    
    tmin = -1000
    tmax = 2000
    normalisedValue = ((m − 102) / (921 − 102)) * (2000 − (-1000)) + (-1000)
    normalisedValue = ((m − 102) / 819) * 2000
    normalisedValue = (m − 102) / 0.4095

    //new sensor (not in use)
    0.045 to 4.5v
    rmin = 9
    rmax = 921
    Sensor reads from -1 to 2 bar
    tmin = 0 (-100?)
    tmax = 300
    normalisedValue = ((m - 9) / (921 - 9)) * (3000 - 0) - 0
  */
  return (m-102)/0.4095;
}


float calculateAfrData(int n){
  //10-20afr
  return ((n * (5.0/1023.0)) * 2) + 10;
}


float readAfrSensor(void){
  float afrData = calculateAfrData(analogRead(A1));
  return afrData;
}


//boost pressure
float readBoostData(void) {
  float absolutePressure = normaliseSensorData(analogRead(A0));
  // Subtract 14.7psi/1bar (pressure at sea level)
  
  // tune this value until sensor shows 0 with the engine off:
  float boostPressure = absolutePressure - 200;

  // Update max and min
  if (boostPressure > boostMax) boostMax = boostPressure;
  if (boostPressure < boostMin) boostMin = boostPressure;

  if (currentPeakMillis - startPeakMillis > peakPeriod){ //reset peakValue;
      boostMax = 0;
      startPeakMillis = currentPeakMillis;
    }

  // Log the history:
  //addSensorHistory(boostPressure);
  return boostPressure;
}

/*
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
*/


// Display graphics
void drawBarGraph(int x, int y, int len, int height, int boostPressure) {
  int peakX = (float(boostMax)/1250) * len;
  u8g2.setDrawColor(1);
  u8g2.drawBox(peakX, y, 1, height);
  if (boostPressure > 0) {
    // Draw the pressure bar behind the graph
    //int barLength = ((float)boostPressure / boostMax) * len;  // bar maxes out at previous peak boost value
    int barLength = (float(boostPressure)/1250) * len;  // bar maxes out at 1.2 bar
    //height = (float(boostPressure)/1000) * (-6);
    u8g2.setDrawColor(2);
    u8g2.drawBox(x, y, barLength, height);
    //u8g2.setDrawColor(1);
  }
}


void drawGauge(int x, int y, int len, int maxHeight, int boostPressure) {
  int barLength = (float(boostPressure)/1000) * len;
  int h = 0;
  int width = 5;
  for(int i = 0; i <= barLength; i+=8){
    x = i;
    h = 1+i/8;
    y = (y + h*0.28)-4; 
    u8g2.setDrawColor(3);
    u8g2.drawBox(x,y,width,h);
  }
}
