#define LOVYANGFX_CONFIG_HPP_

// ===== LovyanGFX Config for Waveshare ESP32-S3-Touch-LCD-2.8 =====
#define LGFX_USE_V1
#define LGFX_BOARD 0  // Let LovyanGFX autodetect

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "display.h"

// Declare display instance
static LGFX display;

// Demo values that cycle through different states
struct DemoData {
  uint16_t rpm = 0;
  uint8_t gear = 0;
  uint16_t speed = 0;
  uint8_t oilTemp = 60;
  uint8_t waterTemp = 60;
  uint8_t oilPressure = 2;
  unsigned long lastUpdate = 0;
};

DemoData demo;
RacecarDisplay* raceDisplay = nullptr;

// Forward declaration
void updateDemoValues();

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("Initializing display...");
  
  // Initialize display
  display.init();
  display.setRotation(1);
  display.fillScreen(TFT_BLACK);
  
  Serial.println("Display initialized");
  Serial.printf("Display size: %d x %d\n", display.width(), display.height());
  
  // Create race display instance with pointer to display
  raceDisplay = new RacecarDisplay(&display);
  raceDisplay->init();
  
  delay(1000);
}

void loop() {
  unsigned long now = millis();
  
  // Update demo values every 50ms for smooth animation
  if (now - demo.lastUpdate > 50) {
    demo.lastUpdate = now;
    updateDemoValues();
  }
  
  // Update display
  if (raceDisplay) {
    raceDisplay->updateDisplay(demo.rpm, demo.gear, demo.speed, 
                               demo.oilTemp, demo.waterTemp, demo.oilPressure);
  }
  
  delay(16);  // ~60 FPS
}

void updateDemoValues() {
  static uint8_t animationCounter = 0;
  animationCounter++;
  
  // Cycle through different demo scenarios
  uint8_t scenario = (animationCounter / 200) % 6;
  
  switch (scenario) {
    case 0:  // Cold start scenario
      demo.rpm = map(animationCounter % 200, 0, 199, 500, 3000);
      demo.gear = (animationCounter % 200 > 100) ? 2 : 1;
      demo.speed = map(animationCounter % 200, 0, 199, 0, 20);
      demo.oilTemp = map(animationCounter % 200, 0, 199, 40, 75);
      demo.waterTemp = map(animationCounter % 200, 0, 199, 50, 80);
      demo.oilPressure = map(animationCounter % 200, 0, 199, 1, 3);
      break;
      
    case 1:  // Normal operation
      demo.rpm = 2500 + (animationCounter % 100) * 10;
      demo.gear = 3;
      demo.speed = 60 + (animationCounter % 50);
      demo.oilTemp = 85 + (animationCounter % 30);
      demo.waterTemp = 85 + (animationCounter % 20);
      demo.oilPressure = 3 + ((animationCounter % 20) / 10);
      break;
      
    case 2:  // Hard acceleration
      demo.rpm = map(animationCounter % 200, 0, 199, 3000, 7500);
      demo.gear = 2;
      demo.speed = map(animationCounter % 200, 0, 199, 30, 80);
      demo.oilTemp = 90;
      demo.waterTemp = 88;
      demo.oilPressure = 4 + (animationCounter % 20) / 5;
      break;
      
    case 3:  // Overheating scenario
      demo.rpm = 2000 + (animationCounter % 100) * 20;
      demo.gear = 4;
      demo.speed = 100 + (animationCounter % 30);
      demo.oilTemp = 100 + (animationCounter % 30);
      demo.waterTemp = 100 + (animationCounter % 25);
      demo.oilPressure = 3 + ((animationCounter % 20) / 10);
      break;
      
    case 4:  // Low oil pressure
      demo.rpm = 5000 + (animationCounter % 100) * 20;
      demo.gear = 5;
      demo.speed = 120 + (animationCounter % 40);
      demo.oilTemp = 88;
      demo.waterTemp = 82;
      demo.oilPressure = 1;
      break;
      
    case 5:  // Cool down
      demo.rpm = map(animationCounter % 200, 0, 199, 2000, 800);
      demo.gear = 3;
      demo.speed = map(animationCounter % 200, 0, 199, 50, 10);
      demo.oilTemp = map(animationCounter % 200, 0, 199, 95, 65);
      demo.waterTemp = map(animationCounter % 200, 0, 199, 90, 70);
      demo.oilPressure = map(animationCounter % 200, 0, 199, 3, 1);
      break;
  }
}
