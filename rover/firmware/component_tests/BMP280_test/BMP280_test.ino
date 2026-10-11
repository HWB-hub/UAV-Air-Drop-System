#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin(0x76)) { // 或 0x77
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BMP280 Test");
}

void loop() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  delay(1000);
}