# Adafruit IO Dashboard Setup Guide

## Step-by-Step Instructions

### 1. Create an Adafruit IO Account

1. Go to [https://io.adafruit.com](https://io.adafruit.com)
2. Click "Get Started for Free"
3. Note your **Username** and **Active Key** (found under My Key)

### 2. Create Feeds

Navigate to **Feeds → New Feed** and create:

| # | Feed Name | Key (auto-generated) |
|---|-----------|---------------------|
| 1 | `temperature` | `temperature` |
| 2 | `humidity` | `humidity` |
| 3 | `luminance` | `luminance` |
| 4 | `led` | `led` |
| 5 | `automatic` | `automatic` |

### 3. Create a Dashboard

1. Navigate to **Dashboards → New Dashboard**
2. Name it: `Farm Automation`
3. Click **Create**

### 4. Add Dashboard Blocks

Click the **+** (gear icon) on the dashboard to add blocks:

#### Block 1: Temperature Gauge
1. Select **Gauge**
2. Choose feed: `temperature`
3. Settings:
   - Min: `0`
   - Max: `60`
   - Label: `Temperature (°C)`
   - Color: `Red`
4. Click **Save**

#### Block 2: Humidity Gauge
1. Select **Gauge**
2. Choose feed: `humidity`
3. Settings:
   - Min: `0`
   - Max: `100`
   - Label: `Humidity (%)`
   - Color: `Blue`
4. Click **Save**

#### Block 3: Temperature Line Chart
1. Select **Line Chart**
2. Choose feed: `temperature`
3. Settings:
   - Line Color: `Red`
   - X-Axis: `24 Hours`
   - Y-Axis: Auto
4. Click **Save**

#### Block 4: Humidity Line Chart
1. Select **Line Chart**
2. Choose feed: `humidity`
3. Settings:
   - Line Color: `Blue`
   - X-Axis: `24 Hours`
   - Y-Axis: Auto
4. Click **Save`

#### Block 5: Luminance Gauge
1. Select **Gauge**
2. Choose feed: `luminance`
3. Settings:
   - Min: `0`
   - Max: `4095`
   - Label: `Light Level`
   - Color: `Yellow`
4. Click **Save`

#### Block 6: LED Control Toggle
1. Select **Toggle**
2. Choose feed: `led`
3. Settings:
   - ON Text: `Bulb ON`
   - OFF Text: `Bulb OFF`
4. Click **Save**

> **Important:** This toggle controls the light bulb in **Manual Mode** only.

#### Block 7: Auto/Manual Mode Toggle
1. Select **Toggle**
2. Choose feed: `automatic`
3. Settings:
   - ON Text: `Manual`
   - OFF Text: `Auto`
4. Click **Save`

> When set to **Manual** (ON), the LED toggle controls the bulb.  
> When set to **Auto** (OFF), the photoresistor controls the bulb.

#### Block 8: Temperature Indicator
1. Select **Indicator**
2. Choose feed: `temperature`
3. Settings:
   - When value is `> 35`: Show **RED** ("Too Hot!")
   - Otherwise: Show **GREEN** ("OK")
4. Click **Save**

### 5. Arrange Your Dashboard

Drag and resize blocks to create a clean layout:

```
┌─────────────────────────────────────────────────┐
│                Farm Automation                   │
├──────────────┬──────────────┬───────────────────┤
│  🌡️ Temp     │  💧 Humidity │  ☀️ Luminance     │
│   [Gauge]    │   [Gauge]    │   [Gauge]         │
├──────────────┴──────────────┴───────────────────┤
│         📈 Temperature History (Line Chart)      │
├─────────────────────────────────────────────────┤
│         📈 Humidity History (Line Chart)         │
├─────────────────────┬───────────────────────────┤
│  💡 Bulb Control    │  🔄 Mode Select           │
│   [Toggle]          │   [Toggle]                │
│                     │  Auto ←→ Manual           │
├─────────────────────┴───────────────────────────┤
│  🚨 Temperature Status [Indicator]              │
└─────────────────────────────────────────────────┘
```

### 6. Set Up Alerts (Optional)

1. Go to **Alerts → New Alert**
2. Create alerts for:
   - Temperature > 40°C: "High temperature warning!"
   - Humidity < 20%: "Very low humidity!"
   - Device offline for > 5 minutes

### 7. Test Your Setup

1. Upload the firmware to your ESP32
2. Open Serial Monitor at 115200 baud
3. Verify connection: "Connected to Adafruit IO!"
4. Check that data appears on your dashboard
5. Test the toggles to control the bulb

---

## Troubleshooting Dashboard Issues

| Issue | Solution |
|-------|----------|
| No data appearing | Check IO_USERNAME and IO_KEY in config.h |
| Feed not found | Ensure feed names match exactly (case-sensitive) |
| Toggle not working | Check that message handlers are registered |
| Charts not updating | Verify feed data is being pushed (check Serial Monitor) |
| Rate limit errors | Adafruit IO free tier: max 30 data points/minute |

---

## Adafruit IO Rate Limits (Free Tier)

| Limit | Value |
|-------|-------|
| Data points per minute | 30 |
| Feeds | 10 |
| Dashboards | 5 |
| Data history | 30 days |

Our firmware sends data every 10 seconds (6 per minute per feed = 18 total), which is within limits.
