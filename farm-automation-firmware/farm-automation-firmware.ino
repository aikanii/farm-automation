/*
 * Farm Automation System - Main Firmware
 * 
 * Group 5 - Lambda
 * Farm Automation System for Small-Scale Enclosed Farms
 * 
 * Description:
 *   An IoT-based farm automation system using ESP32 that monitors
 *   temperature, humidity, and light levels. It provides automatic
 *   and manual control of lighting, with real-time data displayed
 *   on an LCD and uploaded to Adafruit IO cloud dashboard.
 * 
 * Hardware:
 *   - ESP32 Development Board
 *   - DHT11 Temperature & Humidity Sensor
 *   - Photoresistor (LDR) for light detection
 *   - Red LED (warning indicator)
 *   - Blue LED (cool condition indicator)
 *   - Light Bulb with relay (grow light)
 *   - 16x2 I2C LCD Display
 * 
 * Features:
 *   - Real-time temperature, humidity, and luminance monitoring
 *   - Automatic light control based on ambient light levels
 *   - Manual light override via Adafruit IO dashboard
 *   - Visual LED indicators for environmental conditions
 *   - LCD display with sensor readings and system status
 *   - Auto-reconnection to WiFi and Adafruit IO
 *   - Sensor error handling with retry logic
 *   - Non-blocking timing for responsive operation
 * 
 * Version: 2.0.0
 * Last Updated: 2026-09-21
 */

// ============================================================================
// INCLUDES
// ============================================================================
#include "config.h"
#include "AdafruitIO_WiFi.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// ============================================================================
// AIRLIFT / SPI WIFI SUPPORT
// ============================================================================
#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) || \
    defined(ADAFRUIT_PYPORTAL)
  #if !defined(SPIWIFI_SS)
    #define SPIWIFI     SPI
    #define SPIWIFI_SS  10
    #define NINA_ACK    9
    #define NINA_RESETN 6
    #define NINA_GPIO0  -1
  #endif
  AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS,
                     SPIWIFI_SS, NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
  AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#endif

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
DHT_Unified dht(PIN_DHT_SENSOR, DHT_TYPE);
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// ============================================================================
// ADAFRUIT IO FEEDS
// ============================================================================
AdafruitIO_Feed *feedTemperature = io.feed(FEED_TEMPERATURE);
AdafruitIO_Feed *feedHumidity    = io.feed(FEED_HUMIDITY);
AdafruitIO_Feed *feedLuminance   = io.feed(FEED_LUMINANCE);
AdafruitIO_Feed *feedLed         = io.feed(FEED_LED);
AdafruitIO_Feed *feedAutomatic   = io.feed(FEED_AUTOMATIC);

// ============================================================================
// STATE VARIABLES
// ============================================================================
// Control mode: 0 = automatic, 1 = manual
volatile int controlMode = 0;

// Manual bulb state: 0 = off, 1 = on
volatile int manualBulbState = 0;

// Sensor data
float currentTemperature = NAN;
float currentHumidity    = NAN;
int   currentLuminance   = 0;

// Bulb state
bool bulbIsOn = false;

// Custom LCD characters
byte thermometerIcon[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01010,
  0b01110,
  0b11111,
  0b11111,
  0b01110
};

byte waterDropIcon[8] = {
  0b00100,
  0b00100,
  0b01010,
  0b01010,
  0b10001,
  0b10001,
  0b10001,
  0b01110
};

byte sunIcon[8] = {
  0b00000,
  0b10101,
  0b01110,
  0b11111,
  0b01110,
  0b10101,
  0b00000,
  0b00000
};

// ============================================================================
// TIMING VARIABLES
// ============================================================================
unsigned long lastSensorRead = 0;
unsigned long lastLcdUpdate  = 0;
unsigned long lastCloudPush  = 0;

// Connection state
bool isConnected = false;
bool lcdInitialized = false;

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================
void initializePins();
void initializeLcd();
void initializeSensors();
void connectToCloud();
void readSensors();
void updateLeds();
void updateBulb();
void updateDisplay();
void pushDataToCloud();
void handleConnectionLoss();
float readTemperature();
float readHumidity();
int readLuminance();
void printLcdLine(int row, const char* text);
void showSplashScreen();
void showStatus(const char* line1, const char* line2 = "");

// Adafruit IO message handlers
void handleLedMessage(AdafruitIO_Data *data);
void handleAutomaticMessage(AdafruitIO_Data *data);

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Initialize serial for debugging (optional, won't block if no USB)
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("  Farm Automation System v2.0.0"));
  Serial.println(F("  Group 5 - Lambda"));
  Serial.println(F("========================================"));
  Serial.println();

  // Initialize hardware
  initializePins();
  initializeLcd();
  initializeSensors();

  // Show splash screen
  showSplashScreen();
  delay(2000);

  // Connect to Adafruit IO
  connectToCloud();

  Serial.println(F("Setup complete. Entering main loop."));
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  unsigned long currentMillis = millis();

  // Check and maintain connection
  if (io.status() < AIO_CONNECTED) {
    handleConnectionLoss();
    return;
  }

  // Run Adafruit IO client
  io.run();

  // Read sensors at defined interval
  if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentMillis;
    readSensors();
    updateLeds();
    updateBulb();
  }

  // Update LCD at defined interval
  if (currentMillis - lastLcdUpdate >= LCD_UPDATE_INTERVAL) {
    lastLcdUpdate = currentMillis;
    updateDisplay();
  }

  // Push data to cloud at defined interval (same as sensor read)
  if (currentMillis - lastCloudPush >= SENSOR_READ_INTERVAL) {
    lastCloudPush = currentMillis;
    pushDataToCloud();
  }
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

/**
 * Initialize all GPIO pins
 */
void initializePins() {
  Serial.println(F("Initializing pins..."));

  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_BLUE_LED, OUTPUT);
  pinMode(PIN_LIGHT_BULB, OUTPUT);
  pinMode(PIN_PHOTORESISTOR, INPUT);

  // Ensure all outputs start OFF
  digitalWrite(PIN_RED_LED, LOW);
  digitalWrite(PIN_BLUE_LED, LOW);
  digitalWrite(PIN_LIGHT_BULB, LOW);

  Serial.println(F("Pins initialized."));
}

/**
 * Initialize the LCD display
 */
void initializeLcd() {
  Serial.println(F("Initializing LCD..."));

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcdInitialized = true;

  // Create custom characters
  lcd.createChar(0, thermometerIcon);
  lcd.createChar(1, waterDropIcon);
  lcd.createChar(2, sunIcon);

  Serial.println(F("LCD initialized."));
}

/**
 * Initialize the DHT sensor
 */
void initializeSensors() {
  Serial.println(F("Initializing DHT sensor..."));

  dht.begin();

  // Print sensor details (for debugging)
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.print(F("Temperature sensor: "));
  Serial.println(sensor.name);
  Serial.print(F("  Max value: "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));

  dht.humidity().getSensor(&sensor);
  Serial.print(F("Humidity sensor: "));
  Serial.println(sensor.name);
  Serial.print(F("  Max value: "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));

  Serial.println(F("Sensors initialized."));
}

// ============================================================================
// CONNECTION FUNCTIONS
// ============================================================================

/**
 * Connect to WiFi and Adafruit IO
 */
void connectToCloud() {
  Serial.println(F("Connecting to Adafruit IO..."));
  showStatus("Connecting to", "Adafruit IO...");

  lcd.setCursor(0, 1);

  // Register message handlers BEFORE connecting
  feedLed->onMessage(handleLedMessage);
  feedAutomatic->onMessage(handleAutomaticMessage);

  // Start connection
  io.connect();

  unsigned long connectStart = millis();
  int dotCount = 0;

  // Wait for connection with progress dots
  while (io.status() < AIO_CONNECTED) {
    // Show progress dots on LCD
    lcd.setCursor(dotCount, 1);
    lcd.print(".");
    dotCount++;
    if (dotCount >= LCD_COLUMNS) {
      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear line
      dotCount = 0;
    }

    // Read light sensor while connecting (responsive)
    int lux = analogRead(PIN_PHOTORESISTOR);
    if (lux <= LUX_DARK_THRESHOLD) {
      digitalWrite(PIN_LIGHT_BULB, HIGH);
    } else {
      digitalWrite(PIN_LIGHT_BULB, LOW);
    }

    // Check for timeout
    if (millis() - connectStart > WIFI_CONNECT_TIMEOUT) {
      Serial.println(F("Connection timeout! Will retry..."));
      showStatus("Connection", "Timeout! Retry..");
      delay(RECONNECT_DELAY);
      io.connect();
      connectStart = millis();
      dotCount = 0;
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }

    delay(500);
  }

  // Connection successful
  isConnected = true;
  Serial.println(F("Connected to Adafruit IO!"));
  showStatus("CONNECTED!", "");
  delay(1500);

  // Fetch current feed values
  feedLed->get();
  feedAutomatic->get();

  lcd.clear();
}

/**
 * Handle connection loss and attempt reconnection
 */
void handleConnectionLoss() {
  if (isConnected) {
    Serial.println(F("Connection lost!"));
    isConnected = false;
  }

  showStatus("DISCONNECTED!", "Reconnecting...");
  Serial.println(F("Attempting reconnection..."));

  delay(RECONNECT_DELAY);
  connectToCloud();
}

// ============================================================================
// SENSOR READING FUNCTIONS
// ============================================================================

/**
 * Read all sensors and update global state
 */
void readSensors() {
  float temp = readTemperature();
  float hum  = readHumidity();
  int lux    = readLuminance();

  // Only update if we got valid readings
  if (!isnan(temp)) {
    currentTemperature = temp;
  } else {
    Serial.println(F("WARNING: Temperature reading failed, keeping last value"));
  }

  if (!isnan(hum)) {
    currentHumidity = hum;
  } else {
    Serial.println(F("WARNING: Humidity reading failed, keeping last value"));
  }

  currentLuminance = lux;

  // Print to serial for debugging
  Serial.print(F("T: "));
  Serial.print(currentTemperature);
  Serial.print(F("°C | H: "));
  Serial.print(currentHumidity);
  Serial.print(F("% | L: "));
  Serial.print(currentLuminance);
  Serial.print(F(" | Mode: "));
  Serial.println(controlMode == 0 ? "AUTO" : "MANUAL");
}

/**
 * Read temperature with retry logic
 * Returns NAN if all attempts fail
 */
float readTemperature() {
  for (int attempt = 0; attempt < SENSOR_READ_RETRIES; attempt++) {
    sensors_event_t event;
    dht.temperature().getEvent(&event);

    if (!isnan(event.temperature)) {
      return event.temperature;
    }

    Serial.print(F("Temp read attempt "));
    Serial.print(attempt + 1);
    Serial.println(F(" failed"));
    delay(SENSOR_RETRY_DELAY);
  }
  return NAN;
}

/**
 * Read humidity with retry logic
 * Returns NAN if all attempts fail
 */
float readHumidity() {
  for (int attempt = 0; attempt < SENSOR_READ_RETRIES; attempt++) {
    sensors_event_t event;
    dht.humidity().getEvent(&event);

    if (!isnan(event.relative_humidity)) {
      return event.relative_humidity;
    }

    Serial.print(F("Humidity read attempt "));
    Serial.print(attempt + 1);
    Serial.println(F(" failed"));
    delay(SENSOR_RETRY_DELAY);
  }
  return NAN;
}

/**
 * Read luminance from photoresistor
 * ESP32 ADC is 12-bit (0-4095)
 */
int readLuminance() {
  // Take multiple readings and average for stability
  long sum = 0;
  const int samples = 5;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(PIN_PHOTORESISTOR);
    delay(10);
  }
  return (int)(sum / samples);
}

// ============================================================================
// CONTROL FUNCTIONS
// ============================================================================

/**
 * Update LED indicators based on current conditions
 * 
 * Red LED ON:  Hot & Dry (high temp, low humidity)
 * Blue LED ON: Cool & Humid (low temp, high humidity)
 * Both OFF:    Comfortable range
 */
void updateLeds() {
  // Guard against invalid sensor data
  if (isnan(currentTemperature) || isnan(currentHumidity)) {
    digitalWrite(PIN_RED_LED, LOW);
    digitalWrite(PIN_BLUE_LED, LOW);
    return;
  }

  bool isHotAndDry = (currentTemperature > TEMP_HIGH_THRESHOLD) &&
                     (currentHumidity < HUMID_LOW_THRESHOLD);

  bool isCoolAndHumid = (currentTemperature < TEMP_LOW_THRESHOLD) &&
                        (currentHumidity > HUMID_HIGH_THRESHOLD);

  if (isHotAndDry) {
    // Warning: Too hot and too dry
    digitalWrite(PIN_RED_LED, HIGH);
    digitalWrite(PIN_BLUE_LED, LOW);
  } else if (isCoolAndHumid) {
    // Cool and humid conditions
    digitalWrite(PIN_RED_LED, LOW);
    digitalWrite(PIN_BLUE_LED, HIGH);
  } else {
    // Comfortable range
    digitalWrite(PIN_RED_LED, LOW);
    digitalWrite(PIN_BLUE_LED, LOW);
  }
}

/**
 * Update light bulb based on control mode
 * 
 * Automatic mode: Bulb follows light sensor (ON when dark)
 * Manual mode: Bulb follows manual command from Adafruit IO
 */
void updateBulb() {
  bool shouldTurnOn = false;

  if (controlMode == 0) {
    // AUTOMATIC MODE
    if (currentLuminance <= LUX_DARK_THRESHOLD) {
      shouldTurnOn = true;
    } else {
      shouldTurnOn = false;
    }
  } else {
    // MANUAL MODE
    shouldTurnOn = (manualBulbState == 1);
  }

  // Update bulb state
  if (shouldTurnOn) {
    digitalWrite(PIN_LIGHT_BULB, HIGH);
    bulbIsOn = true;
  } else {
    digitalWrite(PIN_LIGHT_BULB, LOW);
    bulbIsOn = false;
  }
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

/**
 * Update the LCD with current sensor readings and status
 * 
 * Line 1: Temperature and Humidity
 * Line 2: Bulb status and control mode
 */
void updateDisplay() {
  if (!lcdInitialized) return;

  char line1[LCD_COLUMNS + 1];
  char line2[LCD_COLUMNS + 1];

  // Line 1: Temperature and Humidity
  if (isnan(currentTemperature) || isnan(currentHumidity)) {
    snprintf(line1, sizeof(line1), "Sensor Error!   ");
  } else {
    // Format: T:XX*C H:XX%
    snprintf(line1, sizeof(line1), "T:%2.0f" "\xDF" "C H:%2.0f%%",
             currentTemperature, currentHumidity);
  }

  // Line 2: Bulb status and mode
  // Format: Bulb:ON  AUTO
  const char* bulbStr = bulbIsOn ? "ON " : "OFF";
  const char* modeStr = (controlMode == 0) ? "AUTO" : "MAN ";

  snprintf(line2, sizeof(line2), "Bulb:%-3s %4s", bulbStr, modeStr);

  // Write to LCD
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

/**
 * Show a two-line status message on LCD
 */
void showStatus(const char* line1, const char* line2) {
  if (!lcdInitialized) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

/**
 * Show splash screen on startup
 */
void showSplashScreen() {
  if (!lcdInitialized) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Farm Automation");
  lcd.setCursor(0, 1);
  lcd.print("Group 5 v2.0.0");
}

// ============================================================================
// CLOUD COMMUNICATION
// ============================================================================

/**
 * Push current sensor data to Adafruit IO
 */
void pushDataToCloud() {
  if (!isConnected) return;

  // Only push valid data
  if (!isnan(currentTemperature)) {
    feedTemperature->save(currentTemperature);
  }

  if (!isnan(currentHumidity)) {
    feedHumidity->save(currentHumidity);
  }

  feedLuminance->save(currentLuminance);
}

// ============================================================================
// ADAFRUIT IO MESSAGE HANDLERS
// ============================================================================

/**
 * Handle LED feed messages from Adafruit IO
 * Controls the manual bulb on/off state
 */
void handleLedMessage(AdafruitIO_Data *data) {
  Serial.print(F("LED command received: "));
  Serial.println(data->toString());

  if (data->toPinLevel() == HIGH) {
    manualBulbState = 1;
  } else {
    manualBulbState = 0;
  }
}

/**
 * Handle Automatic/Manual mode toggle from Adafruit IO
 */
void handleAutomaticMessage(AdafruitIO_Data *data) {
  Serial.print(F("Mode command received: "));
  Serial.println(data->toString());

  if (data->toPinLevel() == HIGH) {
    controlMode = 1;  // Manual mode
    Serial.println(F("Switched to MANUAL mode"));
  } else {
    controlMode = 0;  // Automatic mode
    Serial.println(F("Switched to AUTOMATIC mode"));
  }
}
