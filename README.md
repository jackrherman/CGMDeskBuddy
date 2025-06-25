Custom Glowcose “CGMDeskBuddy”
DIY bedside glucose light and alarm using ESP32 + WS2812B ring + buzzer

Watch the full build and walkthrough on YouTube: https://youtu.be/f_32VhqJnVs

Overview
CGMDeskBuddy is a cost-effective, open-source “Glowcose” clone that fetches real-time glucose data from your Nightscout API and displays it on a 24-LED WS2812B ring. Colour-coded ranges and smooth fades give you an at-a-glance status, while buzzer alerts wake you if levels go critically high (magenta flash) or low (red flash), even in “sleep mode.”

Features
Live Nightscout integration (mg/dL → mmol/L conversion)

Seven colour bands + two critical flash modes:

< 3.5 mmol/L → flashing red + buzzer

3.5–4.0 mmol/L → steady orange

4.0–5.0 mmol/L → yellow

5.0–8.0 mmol/L → bright green (ideal range)

8.0–10.0 mmol/L → teal

10.0–14.0 mmol/L → light blue

14.0–18.0 mmol/L → purple

18.0 mmol/L → flashing magenta + buzzer

Smooth fade transitions between non-critical colours

Night-mode switch to disable non-critical lighting while still allowing alarms

Test mode cycles through all colour bands (and alarm flashes) without Wi-Fi, so you can verify wiring and buzzer behavior

Hardware Required
ESP32 DevKit (ESP-32S NodeMCU)

24-LED WS2812B NeoPixel ring

Active buzzer

SPST lever or rocker switch

USB power supply (5 V)

Pinout (left side only)
Function	ESP32 Pin	Notes
NeoPixel Data	GPIO 27	Full I/O, always 3.3 V logic
Buzzer	GPIO 26	Full I/O, uses tone()
Night-mode switch	GPIO 25	INPUT_PULLUP, to GND
5 V Power	VIN	to ring VCC
Ground	GND	common ground for all devices

Setup & Usage
Clone this repo

Install Arduino libraries

Adafruit NeoPixel

ArduinoJson

Configure Wi-Fi & Nightscout

cpp
Copy
Edit
const char* ssid          = "Your_SSID";
const char* password      = "Your_Pass!";
const char* nightscoutURL = "https://your.ns.url/api/v1/entries.json?count=1";
Toggle Test Mode
In the sketch, set

cpp
Copy
Edit
#define TEST_MODE  true   // cycles fake values
#define TEST_MODE  false  // live Nightscout mode
Upload & Monitor

Open Serial Monitor at 115 200 baud to see debug prints

In test mode you’ll see every colour band and buzzer flash

In live mode you’ll see “HTTP code, 200” then “Glucose mmol/L, X.X” followed by the corresponding display

How the Code Works
Wi-Fi & HTTPS

Uses WiFiClientSecure with setInsecure() to fetch JSON over HTTPS

Parsing JSON

Reads sgv field from the latest Nightscout entry

Converts mg/dL → mmol/L by dividing by 18.0

Colour Mapping

getColourForMMOL() returns an RGB triplet for the current range

Fade & Flash Logic

Non-critical ranges fade smoothly via fadeToColour()

Critical lows/highs trigger flashRedEmergency() or flashMagentaEmergency(), each syncing the LED flash with a buzzer tone

Night-mode Switch

When the switch reads HIGH (open), all non-critical lighting is suppressed, but critical alarm flashes still run

For full details, wiring diagrams, and live demo, check out the video: https://youtu.be/f_32VhqJnVs

— Thanks for checking out CGMDeskBuddy! Pull requests and feedback welcome.
