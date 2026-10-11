#include <Adafruit_DPS310.h>
#include <SPI.h>

Adafruit_DPS310 dps;
#define DPS310_CS 10  // 定义 CS 引脚为 D10

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  if (!dps.begin_SPI(DPS310_CS)) {  // 使用 SPI 初始化 DPS310
    Serial.println("Failed to find DPS310");
    while (1) delay(10);
  }
  Serial.println("DPS310 OK!");

  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
}

void loop() {
  sensors_event_t temp_event, pressure_event;

  // 使用 getEvents() 替代 getEvent()
  if (!dps.getEvents(&temp_event, &pressure_event)) {
    Serial.println("Failed to read data");
    return;
  }

  Serial.print("Temperature = ");
  Serial.print(temp_event.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");

  delay(1000);
}