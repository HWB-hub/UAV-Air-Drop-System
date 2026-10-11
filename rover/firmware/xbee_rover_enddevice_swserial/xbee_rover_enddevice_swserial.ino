#include <SPI.h>
#include <Wire.h>
#include <Adafruit_DPS310.h>
#include <Servo.h>
#include <SoftwareSerial.h>  // 引入软件串口库

Adafruit_DPS310 dps;
Servo myServo;

// 引脚定义
const int dpsCS = 10; // 如果使用 SPI
const int lm35Pin = A2;
const int servoPin = 7;
const int motor1IN1 = 4;
const int motor1IN2 = 5;
const int motor2IN3 = 6;
const int motor2IN4 = 8;

// 定义软件串口引脚
const int RX_PIN = 2;  // RX连接到D2
const int TX_PIN = 3;  // TX连接到D3

// 创建软件串口对象
SoftwareSerial XBeeSerial(RX_PIN, TX_PIN);

// MPU6050 I2C地址
const byte MPU_6050_addr = 0x68;

// MPU6050寄存器地址
const byte PWR_MGMT_1 = 0x6B;  // 电源管理1寄存器
const byte ACCEL_XOUT_H = 0x3B; // 加速度计X轴高字节
const byte TEMP_OUT_H = 0x41;   // 温度传感器高字节
const byte GYRO_XOUT_H = 0x43;  // 陀螺仪X轴高字节

// 函数声明
void setMotor1(int in1, int in2);
void setMotor2(int in3, int in4);
void wakeUpMPU6050();
float readLM35Temperature();
float readDPS310Pressure();
float readAccelerometerX();
float readAccelerometerY();
float readAccelerometerZ();
float readTemperature();
float readGyroX();
float readGyroY();
float readGyroZ();
void handleCommand(char command);

void setup() {
    Serial.begin(9600);
    Serial.println("Arduino setup completed.");

    // 初始化软件串口
    XBeeSerial.begin(9600);

    // 初始化 DPS310
    if (!dps.begin_SPI(dpsCS)) {
        Serial.println("Could not find a valid DPS310 sensor, check wiring!");
        while (1);
    }
    Serial.println("DPS310 OK!");
    dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

    // 初始化 I2C
    Wire.begin();

    // 唤醒 MPU6050
    wakeUpMPU6050();

    // 初始化舵机
    myServo.attach(servoPin);
    myServo.write(0);

    // 初始化电机引脚
    pinMode(motor1IN1, OUTPUT);
    pinMode(motor1IN2, OUTPUT);
    pinMode(motor2IN3, OUTPUT);
    pinMode(motor2IN4, OUTPUT);
}

void loop() {
    Serial.println("Starting data collection...");

    // 读取传感器数据
    float temperature = readLM35Temperature();
    float pressure = readDPS310Pressure();

    // 读取加速度计数据
    float ax = readAccelerometerX();
    float ay = readAccelerometerY();
    float az = readAccelerometerZ();

    // 读取温度数据
    float tempMPU = readTemperature();

    // 读取陀螺仪数据
    float gx = readGyroX();
    float gy = readGyroY();
    float gz = readGyroZ();

    // 打印数据到串口监视器
    Serial.print("T:");
    Serial.print(temperature);
    Serial.print(",P:");
    Serial.print(pressure);
    Serial.print(",AX:");
    Serial.print(ax);
    Serial.print(",AY:");
    Serial.print(ay);
    Serial.print(",AZ:");
    Serial.print(az);
    Serial.print(",GX:");
    Serial.print(gx);
    Serial.print(",GY:");
    Serial.print(gy);
    Serial.print(",GZ:");
    Serial.print(gz);
    Serial.print(",MPU Temp:");
    Serial.println(tempMPU);

    // 将传感器数据发送到 XBee
    XBeeSerial.print("T:");
    XBeeSerial.print(temperature);
    XBeeSerial.print(",P:");
    XBeeSerial.print(pressure);
    XBeeSerial.print(",AX:");
    XBeeSerial.print(ax);
    XBeeSerial.print(",AY:");
    XBeeSerial.print(ay);
    XBeeSerial.print(",AZ:");
    XBeeSerial.print(az);
    XBeeSerial.print(",GX:");
    XBeeSerial.print(gx);
    XBeeSerial.print(",GY:");
    XBeeSerial.print(gy);
    XBeeSerial.print(",GZ:");
    XBeeSerial.print(gz);
    XBeeSerial.print(",MPU Temp:");
    XBeeSerial.println(tempMPU);

    // 检查是否有来自XBee的指令
    if (XBeeSerial.available() > 0) {
        char command = XBeeSerial.read();
        handleCommand(command);
    }

    delay(1000);
}

// 电机控制函数
void setMotor1(int in1, int in2) {
    digitalWrite(motor1IN1, in1);
    digitalWrite(motor1IN2, in2);
    Serial.print("Motor 1 - IN1: ");
    Serial.print(in1);
    Serial.print(", IN2: ");
    Serial.println(in2);
}

void setMotor2(int in3, int in4) {
    digitalWrite(motor2IN3, in3);
    digitalWrite(motor2IN4, in4);
    Serial.print("Motor 2 - IN3: ");
    Serial.print(in3);
    Serial.print(", IN4: ");
    Serial.println(in4);
}

// 唤醒 MPU6050
void wakeUpMPU6050() {
    Wire.beginTransmission(MPU_6050_addr);
    Wire.write(PWR_MGMT_1);
    Wire.write(0x00); // 清除睡眠模式
    Wire.endTransmission();
    delay(100); // 等待设备唤醒
}

// 读取 LM35 温度传感器数据
float readLM35Temperature() {
    int sensorValue = analogRead(lm35Pin);
    float voltage = sensorValue * (5.0 / 1023.0);
    return voltage * 100;
}

// 读取 DPS310 压力传感器数据
float readDPS310Pressure() {
    sensors_event_t temp_event, pressure_event;
    if (!dps.getEvents(&temp_event, &pressure_event)) {
        Serial.println("Failed to read DPS310 pressure");
        return 0;
    }
    return pressure_event.pressure;
}

// 读取加速度计数据
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

// 读取温度数据
float readTemperature() {
    Wire.beginTransmission(MPU_6050_addr);
    Wire.write(TEMP_OUT_H);
    Wire.endTransmission();
    Wire.requestFrom(MPU_6050_addr, 2, true);
    int tempRaw = (Wire.read() << 8 | Wire.read());
    return (tempRaw / 340.0) + 36.53;
}

// 读取陀螺仪数据
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

// 处理指令
void handleCommand(char command) {
    switch (command) {
        case 'F': // 前进
            setMotor1(HIGH, LOW);
            setMotor2(HIGH, LOW);
            Serial.println("Motor forward");
            break;
        case 'B': // 后退
            setMotor1(LOW, HIGH);
            setMotor2(LOW, HIGH);
            Serial.println("Motor backward");
            break;
        case 'L': // 左转
            setMotor1(LOW, HIGH);
            setMotor2(HIGH, LOW);
            Serial.println("Motor left");
            break;
        case 'R': // 右转
            setMotor1(HIGH, LOW);
            setMotor2(LOW, HIGH);
            Serial.println("Motor right");
            break;
        case 'U': // 舵机向上 90°
            myServo.write(90);
            Serial.println("Servo up to 90°");
            break;
        default:
            Serial.println("Unknown command");
            break;
    }
}