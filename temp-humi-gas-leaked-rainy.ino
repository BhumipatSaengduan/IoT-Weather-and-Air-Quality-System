#include <ArduinoJson.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <WiFi.h>

/*
  Board Management?
    1. Go to Tools > Board > Boards Manager...
    2. Install "esp32" by Espressif Systems.
      > Note: Also take a look at bottom-right, some notifications might popping up for some manual interventions.
    3. Select your board from top-left > COMx  (The port where your board lives. you don't know yours? well.. you're on your own.)
    4. Search for and choose "ESP32 Dev Module".
  Libraries?
    1. Press CTRL+Shift+I.
    2. Install the "Adafruit Unified Sensor" by Adafruit.
    3. Install the "DHT sensor library" by Adafruit.
    4. Install the "ArduinoJSON" by Benoit Blanchon.
*/

// Sensor Pins
#define PIN_DHT 4
#define PIN_MQ135 0
#define PIN_RAIN 2

// wi-fi Configuration
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* SERVER_URL = "http://<YOUR_PC_IP>:5000/data";

DHT dht(PIN_DHT, DHT11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // wi-fi Connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected: ");
  Serial.println(WiFi.localIP());

  // initialize Sensors
  pinMode(PIN_DHT, INPUT);
  pinMode(PIN_MQ135, INPUT);
  pinMode(PIN_RAIN, INPUT);

  dht.begin();
}


void loop() {
  // put your main code here, to run repeatedly:
  sendData();
  // delay(1.5 * 1000);
  delay(24 * 60 * 60 * 1000);
}

void sendData() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected! Reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  HTTPClient http;

  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gas_detected = digitalRead(PIN_MQ135) == LOW ? 1 : 0;  // inverted
  int rain_detected = digitalRead(PIN_RAIN) == LOW ? 1 : 0;  // inverted

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Create JSON Payload
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;
  jsonDoc["gas_detected"] = gas_detected;
  jsonDoc["rain_detected"] = rain_detected;

  Serial.printf("temperature: %f, humidity: %f, gas_detected: %d, rain_detected: %d\n", temperature, humidity, gas_detected, rain_detected);

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  int httpResponseCode = http.POST(jsonString);
  if (httpResponseCode > 0) {
    Serial.print("Data sent successfully! HTTP Response: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending data: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());
  }

  http.end();
}
