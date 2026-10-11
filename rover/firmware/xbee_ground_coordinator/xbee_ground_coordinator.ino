#include <SoftwareSerial.h>

#define XBEE_TX 2
#define XBEE_RX 3

SoftwareSerial xbeeSerial(XBEE_TX, XBEE_RX); // 创建一个独立的串口用于XBee通信

void setup() {
    Serial.begin(9600); // 用于调试的串口
    xbeeSerial.begin(9600); // XBee的串口
    Serial.println("XBee Coordinator setup completed.");
}

void loop() {
    // 检查是否收到传感器数据
    if (xbeeSerial.available()) {
        String sensorData = xbeeSerial.readStringUntil('\n');
        Serial.println("Received from End Device: " + sensorData);

        // 解析传感器数据
        int tIndex = sensorData.indexOf("T:");
        int pIndex = sensorData.indexOf("P:");
        int axIndex = sensorData.indexOf("AX:");
        int ayIndex = sensorData.indexOf("AY:");
        int azIndex = sensorData.indexOf("AZ:");
        int gxIndex = sensorData.indexOf("GX:");
        int gyIndex = sensorData.indexOf("GY:");
        int gzIndex = sensorData.indexOf("GZ:");
        int mpuTempIndex = sensorData.indexOf("MPU Temp:");

        if (tIndex != -1 && pIndex != -1 && axIndex != -1 && ayIndex != -1 && azIndex != -1 && gxIndex != -1 && gyIndex != -1 && gzIndex != -1 && mpuTempIndex != -1) {
            float temperature = sensorData.substring(tIndex + 2, pIndex).toFloat();
            float pressure = sensorData.substring(pIndex + 2, axIndex).toFloat();
            float ax = sensorData.substring(axIndex + 3, ayIndex).toFloat();
            float ay = sensorData.substring(ayIndex + 3, azIndex).toFloat();
            float az = sensorData.substring(azIndex + 3, gxIndex).toFloat();
            float gx = sensorData.substring(gxIndex + 3, gyIndex).toFloat();
            float gy = sensorData.substring(gyIndex + 3, gzIndex).toFloat();
            float gz = sensorData.substring(gzIndex + 4, mpuTempIndex).toFloat();
            float tempMPU = sensorData.substring(mpuTempIndex + 8).toFloat();

            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.print("°C, Pressure: ");
            Serial.print(pressure);
            Serial.print(" Pa, AX: ");
            Serial.print(ax);
            Serial.print(" g, AY: ");
            Serial.print(ay);
            Serial.print(" g, AZ: ");
            Serial.print(az);
            Serial.print(" g, GX: ");
            Serial.print(gx);
            Serial.print(" °/s, GY: ");
            Serial.print(gy);
            Serial.print(" °/s, GZ: ");
            Serial.print(gz);
            Serial.print(" °/s, MPU Temp: ");
            Serial.print(tempMPU);
            Serial.println("°C");
        }
    }

    // 检查是否收到用户输入的控制指令
    if (Serial.available()) {
        char command = Serial.read();
        xbeeSerial.write(command); // 将指令发送到End Device
        Serial.print("Sent command to End Device: ");
        Serial.println(command);
    }
}