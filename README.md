# Farm Automation System

**Group 5 — Lambda**

An IoT-based farm automation system for small-scale enclosed farms using ESP32, with cloud monitoring via Adafruit IO.

![Farm Automation Device](Actual%20Device.jpg)

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Hardware Requirements](#hardware-requirements)
4. [Wiring Diagram](#wiring-diagram)
5. [Software Setup](#software-setup)
6. [Adafruit IO Dashboard Setup](#adafruit-io-dashboard-setup)
7. [Configuration](#configuration)
8. [How It Works](#how-it-works)
9. [Troubleshooting](#troubleshooting)
10. [Documentation](#documentation)

---

## 🌱 Overview

The Farm Automation System monitors environmental conditions (temperature, humidity, and light levels) in an enclosed farm and provides automated control of grow lighting. Data is displayed locally on an LCD and uploaded to the Adafruit IO cloud dashboard for remote monitoring and control.

The system supports two operating modes:
- **Automatic Mode**: The grow light turns on/off based on ambient light levels detected by the photoresistor.
- **Manual Mode**: The grow light is controlled remotely via the Adafruit IO dashboard.

---

## ✨ Features

| Feature | Description |
|---------|-------------|
| 🌡️ **Temperature Monitoring** | Real-time temperature readings via DHT11 sensor |
| 💧 **Humidity Monitoring** | Real-time humidity readings via DHT11 sensor |
| ☀️ **Light Level Detection** | Ambient light sensing via photoresistor (LDR) |
| 💡 **Automatic Lighting** | Grow light auto-controlled by ambient light levels |
| 🎮 **Manual Override** | Remote manual control via Adafruit IO dashboard |
| 📺 **LCD Display** | Local 16×2 LCD showing sensor data and system status |
| 🔴 **LED Indicators** | Red LED for hot/dry warnings, Blue LED for cool/humid |
| ☁️ **Cloud Dashboard** | Real-time data visualization on Adafruit IO |
| 🔄 **Auto-Reconnect** | Automatic WiFi and cloud reconnection on disconnect |
| ⚡ **Error Handling** | Sensor retry logic and graceful error recovery |
| ⏱️ **Non-Blocking Timing** | Responsive main loop using millis() |

---

## 🔧 Hardware Requirements

| Component | Quantity | Description |
|-----------|----------|-------------|
| ESP32 Dev Board | 1 | Main microcontroller (e.g., ESP32-WROOM-32) |
| DHT11 Sensor | 1 | Temperature & humidity sensor module |
| Photoresistor (LDR) | 1 | Light-dependent resistor for luminance detection |
| Red LED | 1 | 5mm, with 220Ω resistor |
| Blue LED | 1 | 5mm, with 220Ω resistor |
| Light Bulb + Relay | 1 | Grow light with relay module (5V relay recommended) |
| 16×2 I2C LCD | 1 | LCD with I2C backpack (address 0x27) |
| Breadboard | 1 | For prototyping |
| Jumper Wires | ~20 | Male-to-male and male-to-female |
| USB Cable | 1 | Micro-USB or USB-C for ESP32 |
| 5V Power Supply | 1 | For powering relay and peripherals |

---

## 🔌 Wiring Diagram

```
ESP32 Pin Connections:
═══════════════════════════════════════════════════════════════

                    ┌──────────────┐
                    │    ESP32     │
                    │              │
  DHT11 Data ──────│ GPIO 4       │
                    │              │
  Photoresistor ────│ GPIO 33      │  (ADC1 channel)
                    │              │
  Red LED ─────────│ GPIO 5       │  (via 220Ω resistor)
                    │              │
  Blue LED ────────│ GPIO 18      │  (via 220Ω resistor)
                    │              │
  Relay (Bulb) ────│ GPIO 23      │
                    │              │
  LCD SDA ─────────│ GPIO 21      │  (I2C default SDA)
                    │              │
  LCD SCL ─────────│ GPIO 22      │  (I2C default SCL)
                    │              │
                    │ 3V3 ─────────│──── VCC (DHT11, LCD)
                    │ GND ─────────│──── GND (all components)
                    │ 5V/VIN ──────│──── Relay VCC
                    └──────────────┘

DHT11 Wiring:
  VCC ──── 3.3V
  DATA ─── GPIO 4  (add 10kΩ pull-up resistor to VCC)
  GND ──── GND

Photoresistor (LDR) Wiring:
  One pin ── GPIO 33
  Other pin ── GND
  (Use 10kΩ pull-down resistor to GND, or voltage divider)

LEDs:
  Red LED  (+) ── 220Ω ── GPIO 5
  Red LED  (-) ── GND
  Blue LED (+) ── 220Ω ── GPIO 18
  Blue LED (-) ── GND

Relay Module:
  VCC  ──── 5V
  GND  ──── GND
  IN   ──── GPIO 23
  COM  ──── Light Bulb (Live wire)
  NO   ──── Power Supply (Live wire)

LCD (I2C):
  VCC ──── 5V (or 3.3V, check module)
  GND ──── GND
  SDA ──── GPIO 21
  SCL ──── GPIO 22
```

---

## 💻 Software Setup

### Prerequisites

1. **Arduino IDE** (v2.0+) or **PlatformIO**
2. **ESP32 Board Support** installed in Arduino IDE

### Step 1: Install ESP32 Board Support

1. Open Arduino IDE
2. Go to `File → Preferences`
3. Add this URL to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to `Tools → Board → Boards Manager`
5. Search for "esp32" and install **esp32 by Espressif Systems**

### Step 2: Install Required Libraries

Install these libraries via `Sketch → Include Library → Manage Libraries`:

| Library | Author | Purpose |
|---------|--------|---------|
| **Adafruit IO Arduino** | Adafruit | Cloud connectivity |
| **Adafruit MQTT Library** | Adafruit | MQTT protocol |
| **Adafruit Unified Sensor** | Adafruit | Sensor abstraction |
| **DHT sensor library** | Adafruit | DHT11/DHT22 support |
| **LiquidCrystal I2C** | Frank de Brabander | LCD display |
| **Wire** | Built-in | I2C communication |

### Step 3: Configure the Firmware

1. Open `farm-automation-firmware/config.h`
2. Fill in your credentials:

```cpp
// WiFi
#define WIFI_SSID     "YourWiFiName"
#define WIFI_PASS     "YourWiFiPassword"

// Adafruit IO
#define IO_USERNAME   "YourAdafruitUsername"
#define IO_KEY        "YourAdafruitIOKey"
```

3. Adjust pin assignments and thresholds if your wiring differs

### Step 4: Upload

1. Select your ESP32 board: `Tools → Board → ESP32 Dev Module`
2. Select the correct port: `Tools → Port`
3. Click **Upload** (→)

---

## ☁️ Adafruit IO Dashboard Setup

### Step 1: Create Feeds

Go to [io.adafruit.com](https://io.adafruit.com) and create these feeds:

| Feed Name | Key | Description |
|-----------|-----|-------------|
| Temperature | `temperature` | Temperature readings (°C) |
| Humidity | `humidity` | Humidity readings (%) |
| Luminance | `luminance` | Light level (0–4095) |
| LED | `led` | Manual bulb control toggle |
| Automatic | `automatic` | Auto/Manual mode toggle |

### Step 2: Create Dashboard

Create a new dashboard called "Farm Automation" and add these blocks:

| Block Type | Feed | Settings |
|------------|------|----------|
| **Gauge** | temperature | Min: 0, Max: 60, Unit: °C |
| **Gauge** | humidity | Min: 0, Max: 100, Unit: % |
| **Line Chart** | temperature | Time: 24 hours |
| **Line Chart** | humidity | Time: 24 hours |
| **Gauge** | luminance | Min: 0, Max: 4095 |
| **Toggle** | led | ON/OFF for manual bulb control |
| **Toggle** | automatic | ON = Manual, OFF = Auto |
| **Indicator** | temperature | Threshold: > 35 (red warning) |

### Dashboard Preview

![Farm Automation Dashboard](Farm%20Automation%20Dashboard%20(Adafruit%20IO).JPG)

---

## ⚙️ Configuration

All configuration is centralized in `config.h`. Key settings:

### Thresholds

| Setting | Default | Description |
|---------|---------|-------------|
| `TEMP_HIGH_THRESHOLD` | 35°C | Temperature above which red LED activates |
| `TEMP_LOW_THRESHOLD` | 30°C | Temperature below which blue LED may activate |
| `HUMID_LOW_THRESHOLD` | 35% | Humidity below which red LED activates |
| `HUMID_HIGH_THRESHOLD` | 60% | Humidity above which blue LED may activate |
| `LUX_DARK_THRESHOLD` | 100 | Light level below which auto-mode turns on bulb |

### Timing

| Setting | Default | Description |
|---------|---------|-------------|
| `SENSOR_READ_INTERVAL` | 10,000ms | How often sensors are read |
| `LCD_UPDATE_INTERVAL` | 2,000ms | How often the LCD refreshes |
| `RECONNECT_DELAY` | 5,000ms | Delay between reconnection attempts |

---

## 🔄 How It Works

### System Flow

```
┌─────────────┐     ┌──────────────┐     ┌──────────────┐
│   DHT11     │────▶│              │────▶│   LCD 16x2   │
│  Temp/Humid │     │              │     │   Display     │
└─────────────┘     │              │     └──────────────┘
                    │              │
┌─────────────┐     │    ESP32     │     ┌──────────────┐
│Photoresistor│────▶│              │────▶│   Red LED    │
│  (Light)    │     │              │     │   Blue LED   │
└─────────────┘     │              │     └──────────────┘
                    │              │
┌─────────────┐     │              │     ┌──────────────┐
│  Adafruit   │◀───▶│              │────▶│  Light Bulb  │
│  IO Cloud   │     │              │     │  (via Relay) │
└─────────────┘     └──────────────┘     └──────────────┘
```

### LED Indicator Logic

| Condition | Red LED | Blue LED | Meaning |
|-----------|---------|----------|---------|
| Temp > 35°C **AND** Humidity < 35% | **ON** | OFF | ⚠️ Hot & Dry Warning |
| Temp < 30°C **AND** Humidity > 60% | OFF | **ON** | ❄️ Cool & Humid |
| 30–35°C **AND** 35–60% Humidity | OFF | OFF | ✅ Comfortable |

### Bulb Control Logic

| Mode | Light Level | Bulb State |
|------|-------------|------------|
| Automatic | Dark (≤ 100) | **ON** |
| Automatic | Bright (> 100) | OFF |
| Manual | Any | Controlled by dashboard toggle |

---

## 🔍 Troubleshooting

### Common Issues

| Problem | Possible Cause | Solution |
|---------|---------------|----------|
| LCD shows nothing | Wrong I2C address | Try address `0x3F` in `config.h` |
| LCD shows garbage | I2C timing issue | Check SDA/SCL connections |
| Temperature = NaN | DHT11 not connected | Verify wiring, check pull-up resistor |
| Can't connect to WiFi | Wrong credentials | Double-check SSID and password in `config.h` |
| Can't connect to Adafruit IO | Wrong IO key | Verify IO_USERNAME and IO_KEY |
| Bulb won't turn on | Relay wiring issue | Check relay module connections |
| Erratic light readings | LDR placement | Shield from direct bulb light |

### Serial Monitor

Connect at **115200 baud** to see debug output:
```
========================================
  Farm Automation System v2.0.0
  Group 5 - Lambda
========================================

Initializing pins...
Pins initialized.
Initializing LCD...
LCD initialized.
Initializing DHT sensor...
Sensors initialized.
Connecting to Adafruit IO...
Connected to Adafruit IO!
Setup complete. Entering main loop.
T: 28.00°C | H: 55.00% | L: 245 | Mode: AUTO
```

---

## 📚 Documentation

- [Final Lab Report (PDF)](<(Group%205%20-%20Lambda)%20Documentation%20-%20Final%20Lab%20Act.pdf>)
- [Original Sketch (txt)](group%205%20final%20project%20sketch.txt)

---

## 📁 Project Structure

```
farm-automation/
├── README.md                          # This file
├── farm-automation-firmware/
│   ├── farm-automation-firmware.ino   # Main firmware source code
│   └── config.h                       # Configuration file
├── dashboard/
│   └── dashboard-setup.md             # Adafruit IO dashboard guide
├── docs/
│   └── changelog.md                   # Version history
├── schematics/
│   └── wiring-diagram.md             # Detailed wiring guide
├── Actual Device.jpg                  # Photo of the built device
├── Farm Automation Dashboard (Adafruit IO).JPG
├── (Group 5 - Lambda) Documentation - Final Lab Act.pdf
└── group 5 final project sketch.txt   # Original Arduino sketch
```

---

## 📄 License

This project is for educational purposes. Created by Group 5 — Lambda.

---

## 👥 Group 5 — Lambda

Farm Automation System — Final Lab Project
