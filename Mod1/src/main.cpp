/**
 * BMW E36 Digital Dashboard - Main Entry Point
 * Hardware: Waveshare ESP32-S3-Touch-LCD-2.8B
 * 
 * Reads analog sensors via dual ADS1115 I2C ADCs,
 * calculates gear position from RPM/VSS ratio,
 * displays data on ST7701S RGB display, and
 * drives NeoPixel shift light strip.
 */

#include <Arduino.h>
#include <Wire.h>

// =============================================================================
// Configuration
// =============================================================================

// I2C Pins (shared with touch controller)
#define I2C_SDA         15
#define I2C_SCL         7

// Signal Input Pins
#define PIN_RPM         4     // RPM signal (interrupt)
#define PIN_VSS         0     // Vehicle speed sensor (interrupt) - careful: strapping pin

// Shift Light Output
#define PIN_SHIFTLIGHT  43    // NeoPixel data
#define SHIFTLIGHT_LEDS 16    // Number of LEDs in strip

// Timing Intervals
#define UPDATE_INTERVAL_MS  50    // Main loop update rate
#define DISPLAY_INTERVAL_MS 100   // Display refresh rate
#define GEAR_CALC_INTERVAL_MS 100 // Gear calculation rate

// =============================================================================
// Volatile Counters (ISR-accessed)
// =============================================================================

volatile uint32_t rpmCount = 0;
volatile uint32_t vssCount = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// =============================================================================
// ISR Routines
// =============================================================================

void IRAM_ATTR rpmIsr() {
    portENTER_CRITICAL_ISR(&mux);
    rpmCount++;
    portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR vssIsr() {
    portENTER_CRITICAL_ISR(&mux);
    vssCount++;
    portEXIT_CRITICAL_ISR(&mux);
}

// =============================================================================
// Sensor Reading Functions (Simulated for now - will use ADS1115)
// =============================================================================

float readOilTemp() {
    // Placeholder - will read from ADS1115
    return 180.0 + (rand() % 20);
}

float readOilPressure() {
    // Placeholder - will read from ADS1115
    return 40.0 + (rand() % 10);
}

float readCoolantTemp() {
    // Placeholder - will read from ADS1115
    return 195.0 + (rand() % 15);
}

// =============================================================================
// RPM and Speed Calculation
// =============================================================================

uint32_t calculateRpm(uint32_t pulseCount, uint32_t intervalMs) {
    // RPM = (pulses / pulses_per_rev) * (60000 / interval_ms)
    return (pulseCount * 30000) / intervalMs;
}

float calculateSpeed(uint32_t pulseCount, uint32_t intervalMs) {
    // E36 VSS: ~8000 pulses per mile (varies by diff ratio)
    const float pulsesPerMile = 8000.0;
    float miles = pulseCount / pulsesPerMile;
    float hours = intervalMs / 3600000.0;
    return miles / hours;  // MPH
}

int calculateGear(uint32_t rpm, float mph) {
    if (mph < 1.0 || rpm < 500) return 0;  // Neutral/Stopped
    
    float ratio = (float)rpm / mph;
    
    // E36 gear ratios (stock differential)
    if (ratio > 100)     return 1;
    else if (ratio > 65) return 2;
    else if (ratio > 48) return 3;
    else if (ratio > 38) return 4;
    else if (ratio > 30) return 5;
    return 6;  // Overdrive or high speed
}

// =============================================================================
// Shift Light Control (Simulated - will use NeoPixelBus)
// =============================================================================

void updateShiftLight(uint32_t rpm, uint32_t shiftPoint) {
    // Placeholder - will drive NeoPixel strip
    if (rpm >= shiftPoint) {
        // Flash built-in LED at shift point
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

// =============================================================================
// Setup
// =============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("BMW E36 Digital Dashboard Starting...");
    
    // Initialize I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(400000);
    
    // Initialize built-in LED for debugging
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Attach interrupt handlers
    Serial.println("Attaching interrupt handlers...");
    pinMode(PIN_RPM, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_RPM), rpmIsr, RISING);
    
    pinMode(PIN_VSS, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_VSS), vssIsr, RISING);
    
    Serial.println("System ready.");
}

// =============================================================================
// Main Loop
// =============================================================================

void loop() {
    static uint32_t lastUpdate = 0;
    static uint32_t lastDisplay = 0;
    static uint32_t lastGearCalc = 0;
    
    static uint32_t rpmPulseAccum = 0;
    static uint32_t vssPulseAccum = 0;
    
    static uint32_t currentRpm = 0;
    static float currentSpeed = 0.0;
    static int currentGear = 0;
    
    static float oilTempF = 0;
    static float oilPressurePsi = 0;
    static float coolantTempF = 0;
    
    uint32_t now = millis();
    
    // Accumulate pulse counts every update interval
    if (now - lastUpdate >= UPDATE_INTERVAL_MS) {
        uint32_t rpmPulses, vssPulses;
        
        portENTER_CRITICAL(&mux);
        rpmPulses = rpmCount;
        vssPulses = vssCount;
        rpmCount = 0;
        vssCount = 0;
        portEXIT_CRITICAL(&mux);
        
        rpmPulseAccum += rpmPulses;
        vssPulseAccum += vssPulses;
        
        lastUpdate = now;
    }
    
    // Calculate gear periodically
    if (now - lastGearCalc >= GEAR_CALC_INTERVAL_MS) {
        currentRpm = calculateRpm(rpmPulseAccum, GEAR_CALC_INTERVAL_MS);
        currentSpeed = calculateSpeed(vssPulseAccum, GEAR_CALC_INTERVAL_MS);
        currentGear = calculateGear(currentRpm, currentSpeed);
        
        rpmPulseAccum = 0;
        vssPulseAccum = 0;
        
        lastGearCalc = now;
    }
    
    // Update display and shift light periodically
    if (now - lastDisplay >= DISPLAY_INTERVAL_MS) {
        // Read analog sensors
        oilTempF = readOilTemp();
        oilPressurePsi = readOilPressure();
        coolantTempF = readCoolantTemp();
        
        // Update shift light (shift point at 6500 RPM)
        updateShiftLight(currentRpm, 6500);
        
        // Serial debug output
        Serial.printf("RPM: %5lu | MPH: %5.1f | Gear: %d | Oil: %5.0fF %5.1fpsi | Coolant: %5.0fF\n",
                      currentRpm, currentSpeed, currentGear,
                      oilTempF, oilPressurePsi, coolantTempF);
        
        lastDisplay = now;
    }
    
    delay(1);  // Yield to WiFi/BLE if needed
}
