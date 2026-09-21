# Changelog

All notable changes to the Farm Automation System are documented here.

## [2.0.0] - 2026-09-21

### Added
- **Configuration file** (`config.h`): All settings centralized in one file
- **Non-blocking timing**: Replaced `delay()` with `millis()` for responsive operation
- **Sensor retry logic**: Failed sensor reads are retried up to 3 times before using last known value
- **LCD custom characters**: Thermometer, water drop, and sun icons
- **Serial debug output**: Detailed logging at 115200 baud for troubleshooting
- **Connection timeout**: WiFi connection has a 30-second timeout with retry
- **Sensor averaging**: Luminance readings averaged over 5 samples for stability
- **Graceful degradation**: System continues with last valid sensor readings on failure
- **Splash screen**: Startup display showing project name and version
- **Status display functions**: Reusable LCD display helpers
- **Function prototypes**: All functions declared for better code organization
- **Inline documentation**: Comprehensive comments throughout the code
- **README.md**: Full project documentation with wiring diagrams and setup guide
- **Dashboard setup guide**: Step-by-step Adafruit IO dashboard configuration
- **Changelog**: This file
- **Wiring diagram**: Detailed wiring documentation in schematics folder

### Fixed
- **Uninitialized variables**: `controlMode` and `manualBulbState` now initialized to known state (0)
- **LCD connection loop bug**: Progress dots now properly wrap and clear instead of overrunning
- **LCD clear on reconnect**: Screen properly cleared during reconnection attempts
- **Sensor NaN handling**: Invalid sensor readings no longer overwrite good data
- **Redundant bulb control logic**: Simplified from nested if-else to clean boolean logic
- **Blocking delays in connection**: Light bulb still responds during connection wait
- **Function naming**: `tests()` renamed to `connectToCloud()` for clarity
- **Magic numbers**: All threshold values extracted to named constants in `config.h`
- **Missing pin initialization**: Light bulb pin explicitly set to OUTPUT with initial LOW
- **LCD cursor management**: Proper cursor positioning prevents display artifacts

### Changed
- **Code structure**: Split into logical sections with clear separation of concerns
- **Variable naming**: Descriptive names (`controlMode`, `manualBulbState`, `bulbIsOn`)
- **Feed variable names**: `temperature` → `feedTemperature`, etc. for clarity
- **Display format**: LCD shows control mode (AUTO/MAN) alongside bulb status
- **LED logic**: Uses boolean variables for clearer conditional expressions
- **Connection flow**: More robust reconnection with progress feedback
- **Project structure**: Organized into firmware, docs, dashboard, and schematics folders

### Removed
- **Hardcoded credentials**: Moved to separate `config.h` (never commit actual credentials)
- **Commented-out code**: Removed dead code blocks
- **Blocking delay() calls**: Replaced with millis()-based timing in main loop

## [1.0.0] - Original Release

### Features
- Basic DHT11 temperature and humidity reading
- Photoresistor-based light detection
- Automatic bulb control based on light levels
- Manual bulb control via Adafruit IO
- Red/Blue LED indicators for conditions
- LCD display of readings
- Adafruit IO cloud connectivity
- WiFi auto-reconnection
