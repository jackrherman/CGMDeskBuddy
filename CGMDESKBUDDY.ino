#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

#define LED_PIN        27      // left side GPIO, full io
#define BUZZER_PIN     26      // left side GPIO, full io
#define SWITCH_PIN     25      // manual on/off switch
#define NUM_LEDS       24
#define TEST_MODE      false   // toggle test mode on/off
#define TEST_INTERVAL  2000    // ms between test values in TEST_MODE
#define LIVE_INTERVAL  5000    // ms between live fetches when TEST_MODE is false

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* ssid          = "WIFINAME";
const char* password      = "WIFIPASSWORD";
const char* nightscoutURL = "(COPY AND PASTE ENTIRE NIGHTSCOUT LINK HERE, GET RID OF BRACKETS)api/v1/entries.json?count=1";

unsigned long lastCheckTime = 0;
uint8_t rPrev = 0, gPrev = 0, bPrev = 0;

// test values to hit every colour band
float testMMOLs[] = {2.8, 3.8, 4.5, 6.0, 8.0, 12.0, 16.0, 19.0};
int testIndex = 0;

void setup() {
  Serial.begin(115200);

  if (!TEST_MODE) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to wifi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(",");
    }
    Serial.println("\nWifi connected");
  } else {
    Serial.println("TEST_MODE active, skipping wifi");
  }

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  strip.begin();
  strip.show();  // all off

  // force first cycle immediately
  lastCheckTime = millis() - (TEST_MODE ? TEST_INTERVAL : LIVE_INTERVAL);
}

void loop() {
  unsigned long now = millis();
  unsigned long interval = TEST_MODE ? TEST_INTERVAL : LIVE_INTERVAL;

  if (now - lastCheckTime >= interval) {
    float mmol = TEST_MODE ? getTestMMOL() : getLatestGlucose();
    Serial.print("Glucose mmol/L, ");
    Serial.println(mmol, 1);

    bool switchOff      = digitalRead(SWITCH_PIN) == HIGH;  
    bool isCriticalLow  = mmol < 3.5;
    bool isCriticalHigh = mmol > 18.0;

    uint8_t rNew, gNew, bNew;
    getColourForMMOL(mmol, rNew, gNew, bNew);

    // unified logic: if switch is off, only allow critical flashes
    if (!switchOff || isCriticalLow || isCriticalHigh) {
      if (isCriticalLow) {
        flashRedEmergency();
      }
      else if (isCriticalHigh) {
        flashMagentaEmergency();
      }
      else {
        fadeToColour(rPrev, gPrev, bPrev, rNew, gNew, bNew, 1000);
        rPrev = rNew; gPrev = gNew; bPrev = bNew;
      }
    } else {
      turnOffLeds();
    }

    lastCheckTime = now;
  }
}

void turnOffLeds() {
  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, 0, 0, 0);
  strip.show();
}

void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, r, g, b);
}

void fadeToColour(uint8_t r1, uint8_t g1, uint8_t b1,
                  uint8_t r2, uint8_t g2, uint8_t b2,
                  int duration) {
  for (int i = 0; i <= 50; i++) {
    uint8_t r = r1 + (r2 - r1) * i / 50;
    uint8_t g = g1 + (g2 - g1) * i / 50;
    uint8_t b = b1 + (b2 - b1) * i / 50;
    setAllPixels(r, g, b);
    strip.show();
    delay(duration / 50);
  }
}

void flashRedEmergency() {
  for (int i = 0; i < 6; i++) {
    setAllPixels(255, 0, 0);
    strip.show();
    tone(BUZZER_PIN, 2000);
    delay(250);
    noTone(BUZZER_PIN);
    turnOffLeds();
    delay(250);
  }
}

void flashMagentaEmergency() {
  for (int i = 0; i < 6; i++) {
    setAllPixels(255, 0, 255);
    strip.show();
    tone(BUZZER_PIN, 2000);
    delay(250);
    noTone(BUZZER_PIN);
    turnOffLeds();
    delay(250);
  }
}

void getColourForMMOL(float mmol, uint8_t &r, uint8_t &g, uint8_t &b) {
  if (mmol < 3.5)        { r = 255; g = 0;   b = 0;   }
  else if (mmol < 4.0)   { r = 255; g = 165; b = 0;   }
  else if (mmol < 5.0)   { r = 255; g = 255; b = 0;   }
  else if (mmol < 8.0)   { r = 0;   g = 255; b = 0;   }
  else if (mmol < 10.0)  { r = 0;   g = 255; b = 255; }
  else if (mmol < 14.0)  { r = 0;   g = 0;   b = 255; }
  else if (mmol < 18.0)  { r = 128; g = 0;   b = 128; }
  else                   { r = 255; g = 0;   b = 255; }
}

float getLatestGlucose() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, nightscoutURL);
  int code = http.GET();
  Serial.print("HTTP code, ");
  Serial.println(code);

  if (code == 200) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    auto error = deserializeJson(doc, payload);
    if (!error && doc.size() > 0) {
      float sgv = doc[0]["sgv"];
      http.end();
      return sgv / 18.0;
    }
  }
  Serial.println("Failed to fetch, fallback 5.5");
  http.end();
  return 5.5;
}

float getTestMMOL() {
  float m = testMMOLs[testIndex];
  testIndex = (testIndex + 1) % (sizeof(testMMOLs) / sizeof(testMMOLs[0]));
  return m;
}
