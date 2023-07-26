# arduino based boost and AFR gauge with u8g2 monochrome display

## parts list:
- Bosch LSU4.2 Wideband Sensor with wideband controller
- Defi vacuum/boost pressure sensor
  - or any 5V pressure sensor
  - This project is using a -1/2 Bar vacuum/boost pressure sensor
- Fermion: 1.51” OLED Transparent Display SSD1309 128x64
  - see [this list](https://github.com/olikraus/u8g2/wiki/u8g2setupcpp) for all supported displays
- LM2596 (or equivalent) step-down voltage regulator, for converting from the cars battery (12V+) to the arduino.
  - Tune the converter to around 7V, even though the Arduino takes 5V. This is because of the voltage drop from the V_in to the V_out.
