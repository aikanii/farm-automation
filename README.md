<div align="center">

# 🌾 Farm Automation System

**IoT-powered environmental monitoring and automated lighting control for small-scale enclosed farms**

[![ESP32](https://img.shields.io/badge/Platform-ESP32-E7352C?style=flat-square&logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32)
[![Arduino](https://img.shields.io/badge/Framework-Arduino-00979D?style=flat-square&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Adafruit IO](https://img.shields.io/badge/Cloud-Adafruit%20IO-006699?style=flat-square&logo=adafruit&logoColor=white)](https://io.adafruit.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/Version-2.0.0-blue?style=flat-square)](https://github.com/aikanii/farm-automation)
[![Status](https://img.shields.io/badge/Status-Production%20Ready-brightgreen?style=flat-square)](https://github.com/aikanii/farm-automation)

[Features](#-features) • [Quick Start](#-quick-start) • [Hardware](#-hardware) • [Wiring](#-wiring) • [Configuration](#%EF%B8%8F-configuration) • [Dashboard](#-cloud-dashboard) • [API](#-api-reference) • [Contributing](#-contributing)

</div>

---

## Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [System Architecture](#-system-architecture)
- [Hardware Requirements](#-hardware-requirements)
- [Wiring](#-wiring)
- [Quick Start](#-quick-start)
- [Configuration](#%EF%B8%8F-configuration)
- [Cloud Dashboard](#-cloud-dashboard)
- [API Reference](#-api-reference)
- [Project Structure](#-project-structure)
- [Troubleshooting](#-troubleshooting)
- [Contributing](#-contributing)
- [License](#-license)
- [Authors](#-authors)

---

## 📖 Overview

The Farm Automation System is an IoT solution built on the ESP32 microcontroller that provides real-time environmental monitoring and automated grow-light control for small-scale enclosed farms. The system reads temperature, humidity, and ambient light levels, then intelligently manages lighting while streaming data to the Adafruit IO cloud dashboard for remote monitoring and control.

**Key Capabilities:**
- Continuous monitoring of temperature, humidity, and luminance
- Dual-mode lighting control (automatic + manual override)
- Real-time cloud dashboard with historical data visualization
- Local LCD display with at-a-glance system status
- Visual LED indicators for environmental alerts
- Robust error handling with auto-recovery

---

## ✨ Features

| Category | Feature | Description |
|:---------|:--------|:------------|
| 🌡️ **Sensing** | Temperature & Humidity | DHT11 sensor with 3-retry fault tolerance |
| ☀️ **Sensing** | Ambient Light | Photoresistor with 5-sample averaging |
| 💡 **Control** | Automatic Lighting | Grow light activates when ambient light drops below threshold |
| 🎮 **Control** | Manual Override | Remote bulb control via Adafruit IO dashboard |
| 📺 **Display** | LCD Status Panel | 16×2 I2C LCD showing live readings and system mode |
| 🔴 **Indicators** | LED Alerts | Red LED for hot/dry warnings, Blue LED for cool/humid |
| ☁️ **Cloud** | Adafruit IO | Real-time data upload with historical charts |
| 🔄 **Reliability** | Auto-Reconnect | Seamless WiFi and cloud reconnection on disconnect |
| ⚡ **Reliability** | Error Recovery | Graceful sensor failure handling with last-known-value fallback |
| ⏱️ **Performance** | Non-Blocking | millis()-based timing — no blocking delays in main loop |
| 🔧 **Config** | Centralized Config | All settings in a single `config.h` file |

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        FARM ENVIRONMENT                              │
│                                                                      │
│  ┌──────────┐    ┌──────────────┐    ┌──────────┐    ┌───────────┐  │
│  │  DHT11   │    │Photoresistor │    │ Red LED  │    │ Blue LED  │  │
│  │ Temp/Hum │    │   (LDR)      │    │ Warning  │    │  Status   │  │
│  └────┬─────┘    └──────┬───────┘    └────┬─────┘    └─────┬─────┘  │
│       │                 │                 │                │        │
└───────┼─────────────────┼─────────────────┼────────────────┼────────┘
        │                 │                 │                │
        ▼                 ▼                 ▲                ▲
┌───────────────────────────────────────────────────────────────────────┐
│                            ESP32                                      │
│                                                                       │
│  ┌─────────────────────────────────────────────────────────────────┐  │
│  │                     FIRMWARE v2.0.0                             │  │
│  │                                                                 │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────────┐  │  │
│  │  │  Sensor  │  │ Control  │  │ Display  │  │   Cloud      │  │  │
│  │  │ Manager  │  │  Logic   │  │ Manager  │  │   Client     │  │  │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────────┘  │  │
│  └─────────────────────────────────────────────────────────────────┘  │
│                                                                       │
│       │ GPIO 4        │ GPIO 33      │ GPIO 23                       │
│       ▼               ▼              ▼                               │
│  ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────────────┐  │
│  │  DHT11   │   │   LDR    │   │  Relay   │   │   LCD 16×2 I2C  │  │
│  └──────────┘   └──────────┘   └────┬─────┘   └──────────────────┘  │
└─────────────────────────────────────┼─────────────────────────────────┘
                                      │
                                      ▼
                              ┌──────────────┐
                              │  Light Bulb  │
                              │  (Grow Light)│
                              └──────────────┘

                         ┌──────────────────────┐
         WiFi ◄────────►│    Adafruit IO Cloud  │
                         │                       │
                         │  ┌─────────────────┐  │
                         │  │   Dashboard     │  │
                         │  │  • Gauges       │  │
                         │  │  • Line Charts  │  │
                         │  │  • Toggles      │  │
                         │  │  • Indicators   │  │
                         │  └─────────────────┘  │
                         └──────────────────────┘
```

### Data Flow

```
Sensor Read → Validate → Update Local State → Update LEDs → Update LCD → Push to Cloud
     │                        │
     └── Retry on failure     └── Keep last known good value
```

---

## 🔧 Hardware Requirements

### Bill of Materials

| # | Component | Specification | Qty | Est. Cost |
|---|-----------|---------------|:---:|----------:|
| 1 | ESP32 Dev Board | ESP32-WROOM-32 or similar | 1 | $5–8 |
| 2 | DHT11 Module | Temperature & humidity sensor breakout | 1 | $2–3 |
| 3 | Photoresistor (LDR) | Light-dependent resistor module | 1 | $1–2 |
| 4 | Red LED | 5mm diffused | 1 | $0.10 |
| 5 | Blue LED | 5mm diffused | 1 | $0.10 |
| 6 | 220Ω Resistor | ¼W carbon film | 2 | $0.10 |
| 7 | 10kΩ Resistor | ¼W carbon film (pull-up/pull-down) | 2 | $0.10 |
| 8 | Relay Module | 5V single-channel with optocoupler | 1 | $2–3 |
| 9 | Light Bulb | AC grow light (match relay rating) | 1 | $3–5 |
| 10 | LCD Display | 16×2 with I2C backpack (0x27) | 1 | $3–4 |
| 11 | Breadboard | 830 tie-points | 1 | $2–3 |
| 12 | Jumper Wires | Male-to-male + male-to-female | ~20 | $2 |
| 13 | USB Cable | Micro-USB or USB-C (for ESP32) | 1 | $2 |
| 14 | Power Supply | 5V 2A USB adapter | 1 | $3–5 |

> **Total estimated cost: $25–40 USD**

### ESP32 Pin Allocation

| GPIO | Direction | Function | Component |
|-----:|:---------:|----------|-----------|
| 4 | Input | Digital data | DHT11 sensor |
| 5 | Output | Digital signal | Red LED |
| 18 | Output | Digital signal | Blue LED |
| 21 | Bidirectional | I2C SDA | LCD display |
| 22 | Output | I2C SCL | LCD display |
| 23 | Output | Digital signal | Relay module |
| 33 | Input | Analog read | Photoresistor |

---

## 🔌 Wiring

### Connection Diagram

```
                         ┌──────────────────┐
                         │      ESP32       │
                         │                  │
     DHT11 DATA ────────┤ GPIO 4           │
                         │                  │
     LDR (analog) ──────┤ GPIO 33          │
                         │                  │
     Red LED ─── 220Ω ──┤ GPIO 5           │
                         │                  │
     Blue LED ── 220Ω ──┤ GPIO 18          │
                         │                  │
     Relay IN ──────────┤ GPIO 23          │
                         │                  │
     LCD SDA ───────────┤ GPIO 21          │
                         │                  │
     LCD SCL ───────────┤ GPIO 22          │
                         │                  │
                   3.3V ─┤ 3V3 ──┬── DHT11 VCC
                         │       └── LCD VCC (if 3.3V)
                    5V ─┤ 5V  ──── Relay VCC
                   GND ─┤ GND ──── Common ground
                         └──────────────────┘
```

### DHT11 Wiring

```
     3.3V ─────┬──── VCC (DHT11)
               │
              10kΩ (pull-up, often on module)
               │
     GPIO 4 ───┴──── DATA (DHT11)
               
     GND ──────────── GND (DHT11)
```

### Photoresistor (LDR) Voltage Divider

```
     3.3V �─── 10kΩ ──┬──── GPIO 33
                      │
                    [LDR]
                      │
     GND ─────────────┘
     
     Dark  → LDR resistance ↑ → GPIO 33 voltage ↓ → Low reading
     Light → LDR resistance ↓ → GPIO 33 voltage ↑ → High reading
```

### Relay & Light Bulb

```
     GPIO 23 ────── Relay IN
     
     ┌─────────────────────────────┐
     │        RELAY MODULE         │
     │                             │
     │  COM ──── Light Bulb ──── AC Neutral
     │   NO ──── AC Live           │
     └─────────────────────────────┘
     
     ⚠️  WARNING: Mains voltage (110V/220V) present!
         Use insulated wiring and proper enclosure.
```

---

## 🚀 Quick Start

### Prerequisites

- [Arduino IDE 2.0+](https://www.arduino.cc/en/software) **or** [PlatformIO](https://platformio.org/)
- [ESP32 Board Support](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
- USB cable for ESP32 programming
- Active [Adafruit IO](https://io.adafruit.com/) account (free tier available)

### 1. Install ESP32 Board Support

**Arduino IDE:**
```
File → Preferences → Additional Board Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Tools → Board → Boards Manager → Search "esp32" → Install
```

**PlatformIO:** (uses `platformio.ini` included in project)

### 2. Install Libraries

| Library | Author | Install via |
|---------|--------|-------------|
| Adafruit IO Arduino | Adafruit | Library Manager |
| Adafruit MQTT Library | Adafruit | Library Manager |
| Adafruit Unified Sensor | Adafruit | Library Manager |
| DHT sensor library | Adafruit | Library Manager |
| LiquidCrystal I2C | Frank de Brabander | Library Manager |

```bash
# Or via PlatformIO CLI:
pio pkg install "adafruit/Adafruit IO Arduino"
pio pkg install "adafruit/Adafruit MQTT Library"
pio pkg install "adafruit/Adafruit Unified Sensor"
pio pkg install "adafruit/DHT sensor library"
pio pkg install "marcoschwartz/LiquidCrystal I2C"
```

### 3. Configure Credentials

Edit `farm-automation-firmware/config.h`:

```cpp
// WiFi
#define WIFI_SSID     "your-wifi-ssid"
#define WIFI_PASS     "your-wifi-password"

// Adafruit IO (found at https://io.adafruit.com/settings)
#define IO_USERNAME   "your-adafruit-username"
#define IO_KEY        "your-adafruit-active-key"
```

### 4. Upload Firmware

```
Arduino IDE:
  Tools → Board → ESP32 Dev Module
  Tools → Port → (select your COM port)
  Sketch → Upload

PlatformIO:
  pio run -t upload
```

### 5. Verify

Open Serial Monitor at **115200 baud**:

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
Temperature sensor: DHT11
  Max value: 50.00°C
Humidity sensor: DHT11
  Max value: 100.00%
Sensors initialized.
Connecting to Adafruit IO...
CONNECTED!
Setup complete. Entering main loop.
T: 28°C | H: 55% | L: 245 | Mode: AUTO
```

---

## ⚙️ Configuration

All tunable parameters are in [`farm-automation-firmware/config.h`](farm-automation-firmware/config.h).

### Thresholds

| Parameter | Default | Unit | Description |
|-----------|---------|------|-------------|
| `TEMP_HIGH_THRESHOLD` | `35` | °C | Red LED triggers when exceeded (with low humidity) |
| `TEMP_LOW_THRESHOLD` | `30` | °C | Blue LED triggers when below (with high humidity) |
| `HUMID_LOW_THRESHOLD` | `35` | % | Red LED triggers when below (with high temp) |
| `HUMID_HIGH_THRESHOLD` | `60` | % | Blue LED triggers when exceeded (with low temp) |
| `LUX_DARK_THRESHOLD` | `100` | ADC | Auto-mode activates bulb below this reading |

### Timing

| Parameter | Default | Unit | Description |
|-----------|---------|------|-------------|
| `SENSOR_READ_INTERVAL` | `10000` | ms | Sensor polling frequency |
| `LCD_UPDATE_INTERVAL` | `2000` | ms | LCD refresh frequency |
| `RECONNECT_DELAY` | `5000` | ms | Delay between reconnection attempts |
| `WIFI_CONNECT_TIMEOUT` | `30000` | ms | Max WiFi connection wait time |

### Reliability

| Parameter | Default | Description |
|-----------|---------|-------------|
| `SENSOR_READ_RETRIES` | `3` | Max attempts per sensor read |
| `SENSOR_RETRY_DELAY` | `2000` ms | Delay between retries |

---

## ☁️ Cloud Dashboard

### Adafruit IO Setup

Create the following feeds at [io.adafruit.com](https://io.adafruit.com):

| Feed | Key | Type | Block Type |
|------|-----|------|------------|
| Temperature | `temperature` | Numeric | Gauge (0–60°C) + Line Chart |
| Humidity | `humidity` | Numeric | Gauge (0–100%) + Line Chart |
| Luminance | `luminance` | Numeric | Gauge (0–4095) |
| LED Control | `led` | Boolean | Toggle switch |
| Mode Select | `automatic` | Boolean | Toggle switch |

### Dashboard Layout

```
┌──────────────────────────────────────────────────────────────┐
│                    🌾 Farm Automation                         │
├─────────────────┬─────────────────┬──────────────────────────┤
│  🌡️ Temperature │  💧 Humidity    │  ☀️ Luminance            │
│     ┌─────┐    │     ┌─────┐    │     ┌─────┐              │
│     │ 28° │    │     │ 55% │    │     │ 245 │              │
│     └─────┘    │     └─────┘    │     └─────┘              │
├─────────────────┴─────────────────┴──────────────────────────┤
│  📈 Temperature History ─────────────────────────────────────│
│  ╭─────╮        ╭──╮                                         │
│  │     ╰────────╯  ╰───────                                  │
├──────────────────────────────────────────────────────────────┤
│  📈 Humidity History ────────────────────────────────────────│
│  ───────╮        ╭──────────╮                                │
│         ╰────────╯          ╰────                            │
├────────────────────────┬─────────────────────────────────────┤
│  💡 Bulb Control       │  🔄 Operating Mode                  │
│                        │                                      │
│   [ ON / OFF ]         │   [ AUTO / MANUAL ]                 │
├────────────────────────┴─────────────────────────────────────┤
│  🚨 Status: 🟢 Conditions Normal                             │
└──────────────────────────────────────────────────────────────┘
```

### Control Logic

| Dashboard Toggle | State | Bulb Behavior |
|:----------------|:------|:--------------|
| Mode: **AUTO** (OFF) | Automatic | ON when light < threshold, OFF otherwise |
| Mode: **MANUAL** (ON) | Manual | Follows LED toggle regardless of light level |
| LED: **ON** (Manual mode) | Override | Bulb ON |
| LED: **OFF** (Manual mode) | Override | Bulb OFF |

> **Note:** The `automatic` feed value `LOW` = Auto mode, `HIGH` = Manual mode.

---

## 📡 API Reference

### Adafruit IO Feeds

#### `temperature` (Numeric)

| Property | Value |
|----------|-------|
| Direction | Device → Cloud |
| Unit | °C (Celsius) |
| Update Interval | 10 seconds |
| Range | -40 to 80 |

#### `humidity` (Numeric)

| Property | Value |
|----------|-------|
| Direction | Device → Cloud |
| Unit | % (Relative Humidity) |
| Update Interval | 10 seconds |
| Range | 0 to 100 |

#### `luminance` (Numeric)

| Property | Value |
|----------|-------|
| Direction | Device → Cloud |
| Unit | ADC (0–4095) |
| Update Interval | 10 seconds |
| Range | 0 to 4095 |

#### `led` (Boolean)

| Property | Value |
|----------|-------|
| Direction | Cloud → Device |
| Values | `HIGH` (bulb ON) / `LOW` (bulb OFF) |
| Effective In | Manual mode only |

#### `automatic` (Boolean)

| Property | Value |
|----------|-------|
| Direction | Cloud → Device |
| Values | `LOW` (auto mode) / `HIGH` (manual mode) |

### Serial Monitor Output

```
========================================    ← Startup banner
  Farm Automation System v2.0.0
  Group 5 - Lambda
========================================

Initializing pins...
Pins initialized.
Initializing LCD...
LCD initialized.
Initializing DHT sensor...
Temperature sensor: DHT11                    ← Sensor info
  Max value: 50.00°C
Humidity sensor: DHT11
  Max value: 100.00%
Sensors initialized.
Connecting to Adafruit IO...
CONNECTED!
Setup complete. Entering main loop.

T: 28°C | H: 55% | L: 245 | Mode: AUTO     ← Periodic readings
T: 29°C | H: 53% | L: 198 | Mode: AUTO
LED command received: 1                      ← Remote commands
Switched to MANUAL mode
T: 29°C | H: 53% | L: 198 | Mode: MANUAL
WARNING: Temperature reading failed...       ← Error recovery
T: 29°C | H: 53% | L: 201 | Mode: MANUAL   ← Keeps last good value
```

---

## 📁 Project Structure

```
farm-automation/
│
├── farm-automation-firmware/
│   ├── farm-automation-firmware.ino    # Main firmware source (674 lines)
│   ├── config.h                        # All configuration parameters
│   └── platformio.ini                  # PlatformIO build configuration
│
├── dashboard/
│   ├── dashboard-setup.md              # Adafruit IO setup walkthrough
│   └── arduino-sketch-backup.md        # Original sketch changelog notes
│
├── docs/
│   └── changelog.md                    # Version history & release notes
│
├── schematics/
│   └── wiring-diagram.md              # Detailed wiring documentation
│
├── README.md                           # This file
├── .gitignore                          # Git ignore rules
├── Actual Device.jpg                   # Photo of assembled device
├── Farm Automation Dashboard.JPG       # Screenshot of Adafruit IO dashboard
├── (Group 5) Documentation.pdf         # Lab report (PDF)
└── group 5 final project sketch.txt    # Original v1.0.0 Arduino sketch
```

---

## 🔍 Troubleshooting

### Common Issues

<details>
<summary><strong>LCD shows nothing / garbled characters</strong></summary>

- Verify I2C address: try `0x27` or `0x3F` in `config.h`
- Run an [I2C Scanner sketch](https://playground.arduino.cc/Main/I2cScanner/) to detect the address
- Check SDA → GPIO 21 and SCL → GPIO 22 connections
- Ensure LCD module has power (check backlight)
</details>

<details>
<summary><strong>Temperature / Humidity reads as NaN</strong></summary>

- Verify DHT11 wiring: VCC → 3.3V, DATA → GPIO 4, GND → GND
- Add a 10kΩ pull-up resistor between DATA and VCC (if not on module)
- DHT11 needs ~2 seconds between reads; firmware handles this via timing
- Check Serial Monitor for retry messages
</details>

<details>
<summary><strong>Cannot connect to WiFi / Adafruit IO</strong></summary>

- Double-check `WIFI_SSID` and `WIFI_PASS` in `config.h` (case-sensitive)
- Verify `IO_USERNAME` and `IO_KEY` match your Adafruit IO account
- Ensure WiFi network is 2.4 GHz (ESP32 does not support 5 GHz)
- Check that your Adafruit IO key is the **Active Key** (not legacy)
</details>

<details>
<summary><strong>Light bulb does not respond</strong></summary>

- Check relay module wiring: IN → GPIO 23, VCC → 5V, GND → GND
- Verify relay LED indicator lights up when GPIO 23 goes HIGH
- Test relay with `digitalWrite(23, HIGH)` in Serial Monitor
- Ensure bulb is connected through relay COM and NO terminals
</details>

<details>
<summary><strong>Light sensor readings are erratic</strong></summary>

- Shield the LDR from direct bulb light (causes feedback loop)
- Ensure the 10kΩ pull-down resistor is connected
- Readings are averaged over 5 samples; check wiring if still noisy
</details>

### Debug Checklist

- [ ] Serial Monitor open at **115200 baud**
- [ ] All ground connections share common ground
- [ ] No loose breadboard connections
- [ ] Power supply adequate (5V 2A recommended)
- [ ] Correct board selected in Arduino IDE (`ESP32 Dev Module`)
- [ ] Correct COM port selected

---

## 🤝 Contributing

Contributions are welcome. Please follow these guidelines:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/your-feature`)
3. **Commit** with clear messages (`git commit -m 'Add: your feature'`)
4. **Push** to your branch (`git push origin feature/your-feature`)
5. **Open** a Pull Request

### Code Style

- Use descriptive variable names (no single-letter variables)
- Comment all public functions with `/** Doxygen-style */` blocks
- Keep functions under 50 lines where possible
- Use `#define` constants instead of magic numbers
- Prefix globals with descriptive names (`currentTemperature`, not `t`)

### Reporting Issues

When reporting a bug, include:
- ESP32 board variant
- Arduino IDE / PlatformIO version
- Serial Monitor output (with `SENSOR_READ_RETRIES` at default 3)
- Wiring diagram or photo of your setup

---

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2026 Group 5 - Lambda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 👥 Authors

**Yours truly, MSc**

**NC Balayon, MSc**

**JM Amboang, MSc**

**AL Usman, MSc.**

**MK Asidillo**


Farm Automation System — Final Lab Project

---

<div align="center">

**[⬆ Back to top](#-farm-automation-system)**

</div>
