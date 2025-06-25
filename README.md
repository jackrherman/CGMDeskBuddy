# CGMDeskBuddy (DIY Glowcose Clone)

Build your own bedside CGM light and alarm using an ESP32, WS2812B ring, buzzer and real-time Nightscout data

**Watch the full build video**: https://youtu.be/f_32VhqJnVs

---

## Overview

CGMDeskBuddy fetches your latest Dexcom G6 readings from a Nightscout API, converts mg/dL to mmol/L, and displays a colour-coded status on a 24-LED NeoPixel ring. A buzzer sounds on critically low or high readings—even when you “turn off” the lights with a simple switch.

## Features

- Live Nightscout integration (HTTPS fetch, JSON parse)  
- Colour bands plus critical flash modes:
  - < 3.5 mmol/L → flashing red + buzzer  
  - 3.5–4.0 → orange  
  - 4.0–5.0 → yellow  
  - 5.0–8.0 → bright green (ideal range)  
  - 8.0–10.0 → teal  
  - 10.0–14.0 → light blue  
  - 14.0–18.0 → purple  
  - > 18.0 mmol/L → flashing magenta + buzzer  
- Smooth fade transitions between non-critical colours  
- Night-mode switch to disable normal lighting while still allowing alarms  
- Test mode to cycle through all bands and alarms without Wi-Fi  

## Hardware

- ESP32 DevKit (ESP-32S NodeMCU)  
- 24-LED WS2812B NeoPixel ring  
- Active buzzer  
- SPST lever or rocker switch  
- USB 5 V power supply  

## Pinout (left side of ESP32)

| Function            | ESP32 Pin | Notes                          |
|---------------------|-----------|--------------------------------|
| NeoPixel data (DI)  | GPIO 27   | 3.3 V logic, full I/O          |
| Buzzer              | GPIO 26   | uses `tone()`                  |
| Night-mode switch   | GPIO 25   | `INPUT_PULLUP` to GND          |
| NeoPixel VCC        | VIN       | 5 V power                      |
| Ground              | GND       | common ground                  |

## Configuration

1. Clone this repo  
2. Install Arduino libraries:
   - Adafruit NeoPixel  
   - ArduinoJson  
3. Open `CGMDeskBuddy.ino` and edit these lines:

   ```cpp
   const char* ssid          = "Your_WiFi_SSID";
   const char* password      = "Your_WiFi_Password";
   const char* nightscoutURL = "https://your.nightscout.url/api/v1/entries.json?count=1";
   
   #define TEST_MODE      true    // true = cycle fake values, false = live data
   #define TEST_INTERVAL  2000    // ms between fake-value cycles
   #define LIVE_INTERVAL  60000   // ms between live fetches
