#ifndef DISPLAY_H
#define DISPLAY_H

#include <LovyanGFX.hpp>

class RacecarDisplay {
private:
  LGFX* display;
  
  // Temperature thresholds (in Celsius)
  static constexpr uint8_t OIL_TEMP_COLD = 80;
  static constexpr uint8_t OIL_TEMP_HOT = 100;
  
  static constexpr uint8_t WATER_TEMP_COLD = 70;
  static constexpr uint8_t WATER_TEMP_HOT = 95;
  
  static constexpr uint8_t OIL_PRESSURE_LOW = 1;
  static constexpr uint8_t OIL_PRESSURE_HIGH = 4;
  
  // Colors
  static constexpr uint32_t COLOR_COLD = 0x00FFFF;   // Cyan/Blue
  static constexpr uint32_t COLOR_GOOD = 0x00FF00;   // Green
  static constexpr uint32_t COLOR_HOT = 0xFF0000;    // Red
  static constexpr uint32_t COLOR_TEXT = 0xFFFFFF;   // White
  
  uint32_t getTemperatureColor(uint8_t current, uint8_t coldThresh, uint8_t hotThresh);
  uint32_t getPressureColor(uint8_t current);
  
public:
  RacecarDisplay(LGFX* disp);
  void init();
  void updateDisplay(uint16_t rpm, uint8_t gear, uint16_t speed, 
                     uint8_t oilTemp, uint8_t waterTemp, uint8_t oilPressure);
  void drawBox(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t boxColor, 
               const char* label, const char* value, const char* unit);
};

#endif
