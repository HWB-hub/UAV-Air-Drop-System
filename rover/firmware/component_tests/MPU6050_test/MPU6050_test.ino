#include <Wire.h>

// MPU6050 I2C地址
const byte MPU_6050_addr = 0x68;

// MPU6050寄存器地址
const byte PWR_MGMT_1 = 0x6B;  // 电源管理1寄存器
const byte ACCEL_XOUT_H = 0x3B; // 加速度计X轴高字节
const byte TEMP_OUT_H = 0x41;   // 温度传感器高字节
const byte GYRO_XOUT_H = 0x43;  // 陀螺仪X轴高字节

void setup() {
  Serial.begin(9600); // 初始化串行通信
  Wire.begin();       // 初始化I2C通信

  // 唤醒MPU6050
  wakeUpMPU6050();
}

void loop() {
  // 读取加速度计数据
  float ax = readAccelerometerX();
  float ay = readAccelerometerY();
  float az = readAccelerometerZ();

  // 读取温度数据
  float temperature = readTemperature();

  // 读取陀螺仪数据
  float gx = readGyroX();
  float gy = readGyroY();
  float gz = readGyroZ();

  // 打印数据
  Serial.print("Accelerometer X: ");
  Serial.print(ax);
  Serial.print(" Y: ");
  Serial.print(ay);
  Serial.print(" Z: ");
  Serial.println(az);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Gyro X: ");
  Serial.print(gx);
  Serial.print(" Y: ");
  Serial.print(gy);
  Serial.print(" Z: ");
  Serial.println(gz);

  delay(1000); // 每秒读取一次
}

void wakeUpMPU6050() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(PWR_MGMT_1);
  Wire.write(0x00); // 清除睡眠模式
  Wire.endTransmission();
  delay(100); // 等待设备唤醒
}

float readAccelerometerX() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 2, true);
  return (Wire.read() << 8 | Wire.read()) / 16384.0;
}

float readAccelerometerY() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(ACCEL_XOUT_H + 2);
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 2, true);
  return (Wire.read() << 8 | Wire.read()) / 16384.0;
}

float readAccelerometerZ() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(ACCEL_XOUT_H + 4);
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 2, true);
  return (Wire.read() << 8 | Wire.read()) / 16384.0;
}

float readTemperature() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(TEMP_OUT_H);
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 2, true);
  int tempRaw = (Wire.read() << 8 | Wire.read());
  return (tempRaw / 340.0) + 36.53;
}

float readGyroX() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(GYRO_XOUT_H);
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 2, true);
  return (Wire.read() << 8 | Wire.read()) / 131.0;
}

float readGyroY() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(GYRO_XOUT_H + 2);
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 2, true);
  return (Wire.read() << 8 | Wire.read()) / 131.0;
}

float readGyroZ() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(GYRO_XOUT_H + 4);
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 2, true);
  return (Wire.read() << 8 | Wire.read()) / 131.0;
}