/*
 * config.h - Configuration for Farm Automation System
 * 
 * Group 5 - Lambda
 * Farm Automation System for Small-Scale Enclosed Farms
 * 
 * INSTRUCTIONS:
 * 1. Fill in your WiFi credentials below
 * 2. Fill in your Adafruit IO credentials
 * 3. Adjust pin assignments if your wiring differs
 * 4. Adjust threshold values to suit your farm environment
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASS     "YOUR_WIFI_PASSWORD"

// ============================================================================
// ADAFRUIT IO CONFIGURATION
// ============================================================================
#define IO_USERNAME   "YOUR_ADAFRUIT_IO_USERNAME"
#define IO_KEY        "YOUR_ADAFRUIT_IO_KEY"

// ============================================================================
// ADAFRUIT IO FEED NAMES
// ============================================================================
#define FEED_TEMPERATURE  "temperature"
#define FEED_HUMIDITY     "humidity"
#define FEED_LUMINANCE    "luminance"
#define FEED_LED          "led"
#define FEED_AUTOMATIC    "automatic"

// ============================================================================
// PIN CONFIGURATION (ESP32)
// ============================================================================
#define PIN_DHT_SENSOR    4     // DHT11 data pin
#define PIN_PHOTORESISTOR 33    // Photoresistor (analog input)
#define PIN_RED_LED       5     // Red LED (high temp/low humidity warning)
#define PIN_BLUE_LED      18    // Blue LED (cool/humid conditions)
#define PIN_LIGHT_BULB    23    // Light bulb relay control

// ============================================================================
// LCD CONFIGURATION (I2C)
// ============================================================================
#define LCD_I2C_ADDRESS   0x27  // I2C address of LCD (common: 0x27 or 0x3F)
#define LCD_COLUMNS       16
#define LCD_ROWS          2

// ============================================================================
// SENSOR & THRESHOLD CONFIGURATION
// ============================================================================
#define DHT_TYPE              DHT11       // Sensor type: DHT11 or DHT22

// Temperature thresholds (Celsius)
#define TEMP_HIGH_THRESHOLD   35          // Above this = hot warning (red LED)
#define TEMP_LOW_THRESHOLD    30          // Below this = cool (blue LED if humid)
#define TEMP_COMFORT_MIN      30          // Comfort zone minimum
#define TEMP_COMFORT_MAX      35          // Comfort zone maximum

// Humidity thresholds (percentage)
#define HUMID_LOW_THRESHOLD   35          // Below this = dry warning
#define HUMID_HIGH_THRESHOLD  60          // Above this = humid
#define HUMID_COMFORT_MIN     35          // Comfort zone minimum
#define HUMID_COMFORT_MAX     60          // Comfort zone maximum

// Luminance threshold (analog reading 0-4095)
#define LUX_DARK_THRESHOLD    100         // Below this = dark, turn on bulb

// ============================================================================
// TIMING CONFIGURATION (milliseconds)
// ============================================================================
#define SENSOR_READ_INTERVAL  10000       // How often to read sensors (10 sec)
#define LCD_UPDATE_INTERVAL   2000        // How often to update LCD (2 sec)
#define RECONNECT_DELAY       5000       // Delay before reconnection attempt
#define WIFI_CONNECT_TIMEOUT  30000       // Max time to wait for WiFi (30 sec)

// ============================================================================
// SENSOR VALIDATION
// ============================================================================
#define SENSOR_READ_RETRIES   3           // Number of retries for failed sensor reads
#define SENSOR_RETRY_DELAY    2000        // Delay between sensor retries

#endif // CONFIG_H
