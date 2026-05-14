#include "display.h"

RacecarDisplay::RacecarDisplay(LGFX* disp) : display(disp) {}

void RacecarDisplay::init() {
  display->setTextColor(COLOR_TEXT, TFT_BLACK);
  display->fillScreen(TFT_BLACK);
}

uint32_t RacecarDisplay::getTemperatureColor(uint8_t current, uint8_t coldThresh, uint8_t hotThresh) {
  if (current < coldThresh) {
    return COLOR_COLD;  // Blue - too cold
  } else if (current > hotThresh) {
    return COLOR_HOT;   // Red - too hot
  } else {
    return COLOR_GOOD;  // Green - perfect range
  }
}

uint32_t RacecarDisplay::getPressureColor(uint8_t current) {
  if (current < OIL_PRESSURE_LOW) {
    return COLOR_HOT;   // Red - too low
  } else if (current > OIL_PRESSURE_HIGH) {
    return COLOR_HOT;   // Red - too high
  } else {
    return COLOR_GOOD;  // Green - safe range
  }
}

void RacecarDisplay::drawBox(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t boxColor, 
                              const char* label, const char* value, const char* unit) {
  // Draw border with color
  display->drawRect(x, y, w, h, boxColor);
  display->drawRect(x + 1, y + 1, w - 2, h - 2, boxColor);
  
  // Draw label (small text)
  display->setFont(&fonts::Font2);
  display->setTextSize(1);
  display->setCursor(x + 5, y + 5);
  display->setTextColor(COLOR_TEXT, TFT_BLACK);
  display->print(label);
  
  // Draw value (larger text)
  display->setFont(&fonts::Font7);
  display->setTextSize(1);
  display->setCursor(x + 5, y + 25);
  display->setTextColor(boxColor, TFT_BLACK);
  display->print(value);
  
  // Draw unit (small text)
  display->setFont(&fonts::Font2);
  display->setTextSize(1);
  display->setCursor(x + 5, y + 50);
  display->setTextColor(COLOR_TEXT, TFT_BLACK);
  display->print(unit);
}

void RacecarDisplay::updateDisplay(uint16_t rpm, uint8_t gear, uint16_t speed, 
                                    uint8_t oilTemp, uint8_t waterTemp, uint8_t oilPressure) {
  char buffer[20];
  
  // Clear screen
  display->fillScreen(TFT_BLACK);
  
  // RPM - small at top center
  display->setFont(&fonts::Font4);
  display->setTextSize(1);
  display->setTextColor(COLOR_TEXT, TFT_BLACK);
  display->setCursor(200, 10);
  snprintf(buffer, sizeof(buffer), "RPM: %d", rpm);
  display->print(buffer);
  
  // GEAR - LARGE in center
  display->setFont(&fonts::Font7);
  display->setTextSize(3);
  display->setTextColor(COLOR_TEXT, TFT_BLACK);
  char gearChar[2];
  if (gear == 0) {
    snprintf(gearChar, sizeof(gearChar), "N");
  } else {
    snprintf(gearChar, sizeof(gearChar), "%d", gear);
  }
  
  // Center the gear display
  int16_t gearX = 200;  // Approximate center for 480 width
  int16_t gearY = 180;
  display->setCursor(gearX, gearY);
  display->print(gearChar);
  
  // SPEED - small below gear
  display->setFont(&fonts::Font4);
  display->setTextSize(1);
  display->setTextColor(COLOR_TEXT, TFT_BLACK);
  display->setCursor(200, 350);
  snprintf(buffer, sizeof(buffer), "SPD: %d", speed);
  display->print(buffer);
  
  // Temperature/Pressure boxes at bottom
  // Get colors for each sensor
  uint32_t oilTempColor = getTemperatureColor(oilTemp, OIL_TEMP_COLD, OIL_TEMP_HOT);
  uint32_t waterTempColor = getTemperatureColor(waterTemp, WATER_TEMP_COLD, WATER_TEMP_HOT);
  uint32_t pressureColor = getPressureColor(oilPressure);
  
  // Oil Temp box (left)
  snprintf(buffer, sizeof(buffer), "%d", oilTemp);
  drawBox(20, 420, 140, 110, oilTempColor, "OIL T", buffer, "C");
  
  // Water Temp box (center)
  snprintf(buffer, sizeof(buffer), "%d", waterTemp);
  drawBox(170, 420, 140, 110, waterTempColor, "H2O T", buffer, "C");
  
  // Oil Pressure box (right)
  snprintf(buffer, sizeof(buffer), "%d", oilPressure);
  drawBox(320, 420, 140, 110, pressureColor, "OIL P", buffer, "PSI");
}
