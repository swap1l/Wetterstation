#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <ESPAsyncWebServer.h>
#include "DHT.h"

#define DHT11_PIN 9
#define GAS_PIN 2
#define LED_PIN 8
DHT dht11(DHT11_PIN, DHT11);
Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);

WiFiManager wifiManager;
AsyncWebServer server(80);

#define MAX_POINTS 5
float tempHistory[MAX_POINTS];
float humiHistory[MAX_POINTS];
float gasHistory[MAX_POINTS];
int tempIndex = 0;

bool ledEnabled = true;

void updateLED(float t) {
  if (ledEnabled) {
    if (WiFi.status() != WL_CONNECTED) {
      led.setPixelColor(0, led.Color(255, 0, 0)); // rot
    } else if (t > 30) {
      led.setPixelColor(0, led.Color(255, 100, 0)); // orange
    } else {
      led.setPixelColor(0, led.Color(0, 0, 255)); // blau
    }
  } else {
    led.setPixelColor(0, 0);
  }
  led.show();
}

void setup() {
  Serial.begin(115200);
  dht11.begin();
  led.begin();
  led.show();
  pinMode(GAS_PIN, INPUT);

  if (wifiManager.autoConnect("WetterstationAP")) {
    Serial.println("✅ WLAN verbunden: " + WiFi.localIP().toString());
  }

  // Root: HTML Interface mit Graph und LED-Steuerung
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    String html = R"rawliteral(
      <html><head><meta charset="UTF-8"><title>Wetterstation</title></head><body>
      <h2>🌤 Wetterstation ESP32</h2>
      <p><a href="/led/on">LED Ein</a> | <a href="/led/off">LED Aus</a></p>
      <p><a href="/daten">📈 Verlauf</a> | <a href="http://192.168.130.122/daten.php">📊 Datenbank</a></p>
      <canvas id="chart" width="400" height="200"></canvas>
      <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
      <script>
        async function fetchData() {
          const res = await fetch('/daten');
          const data = await res.json();

          const ctx = document.getElementById('chart').getContext('2d');
          new Chart(ctx, {
            type: 'bar',
            data: {
              labels: ['Temperatur (°C)', 'Luftfeuchtigkeit (%)', 'Gas (%)'],
              datasets: [{
                label: '⌀ Durchschnitt der letzten 5 Werte',
                data: [data.temperatur, data.luftfeuchtigkeit, data.gas],
                backgroundColor: ['orange', 'blue', 'green']
              }]
            },
            options: {
              scales: {
                y: {
                  beginAtZero: true,
                  max: 100
                }
              }
            }
          });
        }
        fetchData();
        </script>

      </body></html>
    )rawliteral";
    req->send(200, "text/html", html);
  });

  // Verlauf als JSON
  server.on("/daten", HTTP_GET, [](AsyncWebServerRequest *req) {
    float tempSum = 0, humiSum = 0, gasSum = 0;
    for (int i = 0; i < MAX_POINTS; i++) {
      tempSum += tempHistory[i];
      humiSum += humiHistory[i];
      gasSum  += gasHistory[i];
    }

    float tempAvg = tempSum / MAX_POINTS;
    float humiAvg = humiSum / MAX_POINTS;
    float gasAvg  = gasSum  / MAX_POINTS;

    String json = "{";
    json += "\"temperatur\":" + String(tempAvg, 2) + ",";
    json += "\"luftfeuchtigkeit\":" + String(humiAvg, 2) + ",";
    json += "\"gas\":" + String(gasAvg, 2);
    json += "}";

    req->send(200, "application/json", json);
  });


  server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *req) {
    ledEnabled = true;
    req->redirect("/");
  });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *req) {
    ledEnabled = false;
    req->redirect("/");
  });

  server.begin();
}

void loop() {
  static unsigned long last = 0;
  if (millis() - last > 10000) {
    last = millis();

    float t = dht11.readTemperature();
    float h = dht11.readHumidity();
    int rawGas = analogRead(GAS_PIN);
    float gasPercent = rawGas / 4095.0 * 100.0;

    if (!isnan(t) && !isnan(h)) {
      Serial.printf("🌡 %.2f °C | 💧 %.2f %% | 🧪 Gas: %.2f %%\n", t, h, gasPercent);

      tempHistory[tempIndex] = t;
      humiHistory[tempIndex] = h;
      gasHistory[tempIndex] = gasPercent;
      tempIndex = (tempIndex + 1) % MAX_POINTS;

      updateLED(t);

      HTTPClient http;
      http.begin("http://192.168.130.122/insert.php");
      http.addHeader("Content-Type", "application/json");
      String json = "{\"temperatur\":" + String(t) +
                    ",\"luftfeuchtigkeit\":" + String(h) +
                    ",\"sensor\":" + String(gasPercent) + "}";

      int code = http.POST(json);
      if (code > 0) {
        Serial.println("✅ DB-Response: " + http.getString());
      } else {
        Serial.printf("❌ Fehler beim Senden: %s\n", http.errorToString(code).c_str());
      }
      http.end();
    }
  }
}
