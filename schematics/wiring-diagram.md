# Wiring Diagram & Schematic

## Complete Wiring Guide

### Power Distribution

```
ESP32 3.3V  ───┬──── DHT11 VCC
               ├──── LCD VCC (if 3.3V compatible)
               │
ESP32 5V/VIN ──┼──── Relay VCC
               └──── LCD VCC (if 5V module)

ESP32 GND   ───┬──── DHT11 GND
               ├──── LCD GND
               ├──── Relay GND
               ├──── Red LED cathode (−)
               ├──── Blue LED cathode (−)
               └──── Photoresistor (one leg)
```

### Signal Connections

#### DHT11 Temperature & Humidity Sensor

```
         ┌──────────┐
         │  DHT11   │
         │  Module  │
         │          │
         │ VCC  GND │
         │  │    │  │
        3.3V GND  DATA──GPIO 4
                    │
                  10kΩ pull-up to 3.3V
                  (often built into module)

Pin mapping:
  DHT11 VCC  → ESP32 3.3V
  DHT11 DATA → ESP32 GPIO 4
  DHT11 GND  → ESP32 GND
```

> **Note:** Many DHT11 breakout boards include the pull-up resistor. If using a bare DHT11, add a 10kΩ resistor between DATA and VCC.

#### Photoresistor (LDR)

```
  3.3V ──── 10kΩ ──┬─── GPIO 33
                    │
                  [LDR]
                    │
                   GND

This forms a voltage divider:
  - When dark: LDR resistance ↑ → voltage at GPIO 33 ↓
  - When bright: LDR resistance ↓ → voltage at GPIO 33 ↑

Pin mapping:
  LDR leg 1  → ESP32 3.3V (through 10kΩ resistor)
  LDR leg 2  → ESP32 GND
  Junction   → ESP32 GPIO 33
```

> **Note:** Some LDR modules have a built-in voltage divider. Connect VCC, GND, and analog output to GPIO 33.

#### LEDs

```
  GPIO 5 ──── 220Ω ──── Red LED (+) ──── Red LED (−) ──── GND

  GPIO 18 ──── 220Ω ──── Blue LED (+) ──── Blue LED (−) ──── GND

Resistor calculation:
  V = 3.3V (ESP32 GPIO high)
  V_LED = 2.0V (typical red/blue LED forward voltage)
  I = 10-20mA (typical LED current)
  R = (3.3V - 2.0V) / 0.015A ≈ 87Ω minimum
  220Ω used for safety and longer LED life
```

#### Relay Module & Light Bulb

```
  ESP32 GPIO 23 ──── Relay IN

  Relay module connections:
  ┌────────────────────────┐
  │     RELAY MODULE       │
  │                        │
  │  VCC  GND  IN          │
  │   │    │   │           │
  │  5V  GND  GPIO 23      │
  │                        │
  │  COM    NO    NC       │
  │   │     │             │
  │   │     └──── AC Live (from power supply)
  │   │                     │
  │   └────── Light Bulb ───┘
  │           (other wire → AC Neutral)
  └────────────────────────┘

  ⚠️ WARNING: Relay switching involves Mains AC voltage!
  - Use proper insulation
  - Never touch exposed wires when powered
  - Consider using a relay module with optocoupler isolation
```

#### LCD Display (I2C)

```
  ┌──────────────┐
  │  LCD 16x2    │
  │  w/ I2C      │
  │              │
  │  VCC GND SDA SCL
  │   │   │   │   │
  │  5V GND  21  22│  (ESP32 default I2C pins)
  └──────────────┘

  I2C Address: 0x27 (most common)
  Alternative: 0x3F (some modules)
  
  If display doesn't work, try scanning I2C bus:
  Use the I2C Scanner sketch to find the correct address
```

### GPIO Summary Table

| GPIO | Function | Component | Direction |
|------|----------|-----------|-----------|
| 4 | Digital In | DHT11 Data | Input |
| 5 | Digital Out | Red LED | Output |
| 18 | Digital Out | Blue LED | Output |
| 21 | I2C SDA | LCD | Bidirectional |
| 22 | I2C SCL | LCD | Output |
| 23 | Digital Out | Relay (Bulb) | Output |
| 33 | Analog In | Photoresistor | Input |

### ESP32 Pin Limitations

⚠️ **Important ESP32 GPIO notes:**

| GPIO | Notes |
|------|-------|
| 0 | Must be HIGH at boot (pull-up) |
| 2 | Must be floating/HIGH at boot |
| 6-11 | Connected to flash, DO NOT USE |
| 12 | Must be LOW at boot (strapping pin) |
| 34-39 | Input only, no pull-up/pull-down |

Our chosen pins (4, 5, 18, 21, 22, 23, 33) are safe and avoid these restrictions.

### Complete Wiring Checklist

- [ ] ESP32 powered via USB or 5V supply
- [ ] DHT11 connected (VCC, DATA→GPIO4, GND)
- [ ] Photoresistor connected with voltage divider to GPIO33
- [ ] Red LED with 220Ω resistor to GPIO5
- [ ] Blue LED with 220Ω resistor to GPIO18
- [ ] Relay module connected (VCC, GND, IN→GPIO23)
- [ ] Light bulb wired through relay COM/NO
- [ ] LCD connected (VCC, GND, SDA→GPIO21, SCL→GPIO22)
- [ ] All GND connections share common ground
- [ ] No short circuits or loose connections
