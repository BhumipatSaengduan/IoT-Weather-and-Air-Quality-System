Male-Female:
  1. ESP32 VCC -> Breadboard VCC
  2. ESP32 GND -> Breadboard GND
  3. Rain  VCC -> Breadboard VCC
  4. Rain  GND -> Breadboard GND
  5. MQ135 VCC -> Breadboard VCC
  6. MQ135 GND -> Breadboard GND
  7. DHT   VCC -> Breadboard VCC
  8. DHT   GND -> Breadboard GND

Female-Female:
  1. Rain  DO  -> Pin G2
  2. MQ135 DO  -> Pin G0
  3. DHT   out -> Pin G4

Run Server:
1. docker build -t iotdataanalysis:latest .
2. docker run -p 0.0.0.0:8000:8000 --rm iotdataanalysis:latest

Flash ESP32:
1. Open Arduino IDE. (Installed from https://www.arduino.cc/)
2. Install the "esp32" boards platform. (Detailed steps in the sketch ino file)
3. Install the "Adafruit Unified Sensor", "DHT sensor library", and the "ArduinoJSON". (Detailed steps in the sketch ino file)
4. Modify WiFi configuration in the code and change `SERVER_URL` to `http://<server_ip>:8000/data`.